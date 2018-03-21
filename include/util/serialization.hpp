#ifndef OSMR_UTIL_SERIALIZATION_HPP
#define OSMR_UTIL_SERIALIZATION_HPP

#include "util/dynamic_graph.hpp"
#include "util/packed_vector.hpp"
#include "util/range_table.hpp"
#include "util/static_graph.hpp"
#include "util/indexed_data.hpp"

#include "storage/io.hpp"
#include "storage/serialization.hpp"

namespace osrm
{
namespace util
{
namespace serialization
{
template <unsigned BlockSize, storage::Ownership Ownership>
void write(storage::io::FileWriter &writer, const util::RangeTable<BlockSize, Ownership> &table)
{
    writer.WriteFrom(table.sum_lengths);
    storage::serialization::write(writer, table.block_offsets);
    storage::serialization::write(writer, table.diff_blocks);
}

template <unsigned BlockSize, storage::Ownership Ownership>
void read(storage::io::FileReader &reader, util::RangeTable<BlockSize, Ownership> &table)
{
    reader.ReadInto(table.sum_lengths);
    storage::serialization::read(reader, table.block_offsets);
    storage::serialization::read(reader, table.diff_blocks);
}

template <unsigned BlockSize, storage::Ownership Ownership>
void write(storage::tar::FileWriter &writer,
           const std::string &name,
           const util::RangeTable<BlockSize, Ownership> &table)
{
    writer.WriteFrom(name + "/sum_lengths.meta", table.sum_lengths);
    storage::serialization::write(writer, name + "/block_offsets", table.block_offsets);
    storage::serialization::write(writer, name + "/diff_blocks", table.diff_blocks);
}

template <unsigned BlockSize, storage::Ownership Ownership>
void read(storage::tar::FileReader &reader,
          const std::string &name,
          util::RangeTable<BlockSize, Ownership> &table)
{
    reader.ReadInto(name + "/sum_lengths.meta", table.sum_lengths);
    storage::serialization::read(reader, name + "/block_offsets", table.block_offsets);
    storage::serialization::read(reader, name + "/diff_blocks", table.diff_blocks);
}

template <typename T, std::size_t Bits, storage::Ownership Ownership>
inline void read(storage::io::FileReader &reader, detail::PackedVector<T, Bits, Ownership> &vec)
{
    reader.ReadInto(vec.num_elements);
    storage::serialization::read(reader, vec.vec);
}

template <typename T, std::size_t Bits, storage::Ownership Ownership>
inline void write(storage::io::FileWriter &writer,
                  const detail::PackedVector<T, Bits, Ownership> &vec)
{
    writer.WriteFrom(vec.num_elements);
    storage::serialization::write(writer, vec.vec);
}

template <typename T, std::size_t Bits, storage::Ownership Ownership>
inline void read(storage::tar::FileReader &reader,
                 const std::string &name,
                 detail::PackedVector<T, Bits, Ownership> &vec)
{
    reader.ReadInto(name + "/number_of_elements.meta", vec.num_elements);
    storage::serialization::read(reader, name + "/packed", vec.vec);
}

template <typename T, std::size_t Bits, storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const detail::PackedVector<T, Bits, Ownership> &vec)
{
    writer.WriteFrom(name + "/number_of_elements.meta", vec.num_elements);
    storage::serialization::write(writer, name + "/packed", vec.vec);
}

template <typename EdgeDataT, storage::Ownership Ownership>
inline void read(storage::io::FileReader &reader, StaticGraph<EdgeDataT, Ownership> &graph)
{
    storage::serialization::read(reader, graph.node_array);
    storage::serialization::read(reader, graph.edge_array);
    graph.number_of_nodes = graph.node_array.size() - 1;
    graph.number_of_edges = graph.edge_array.size();
}

template <typename EdgeDataT, storage::Ownership Ownership>
inline void write(storage::io::FileWriter &writer, const StaticGraph<EdgeDataT, Ownership> &graph)
{
    storage::serialization::write(writer, graph.node_array);
    storage::serialization::write(writer, graph.edge_array);
}

template <typename EdgeDataT>
inline void read(storage::io::FileReader &reader, DynamicGraph<EdgeDataT> &graph)
{
    storage::serialization::read(reader, graph.node_array);
    const auto num_edges = reader.ReadElementCount64();
    graph.edge_list.resize(num_edges);
    for (auto index : irange<std::size_t>(0, num_edges))
    {
        reader.ReadInto(graph.edge_list[index]);
    }
    graph.number_of_nodes = graph.node_array.size();
    graph.number_of_edges = num_edges;
}

template <typename EdgeDataT>
inline void write(storage::io::FileWriter &writer, const DynamicGraph<EdgeDataT> &graph)
{
    storage::serialization::write(writer, graph.node_array);
    writer.WriteElementCount64(graph.number_of_edges);
    for (auto index : irange<std::size_t>(0, graph.number_of_edges))
    {
        writer.WriteFrom(graph.edge_list[index]);
    }
}

template <typename EdgeDataT, storage::Ownership Ownership>
inline void read(storage::tar::FileReader &reader,
                 const std::string &name,
                 StaticGraph<EdgeDataT, Ownership> &graph)
{
    storage::serialization::read(reader, name + "/node_array", graph.node_array);
    storage::serialization::read(reader, name + "/edge_array", graph.edge_array);
    graph.number_of_nodes = graph.node_array.size() - 1;
    graph.number_of_edges = graph.edge_array.size();
}

template <typename EdgeDataT, storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const StaticGraph<EdgeDataT, Ownership> &graph)
{
    storage::serialization::write(writer, name + "/node_array", graph.node_array);
    storage::serialization::write(writer, name + "/edge_array", graph.edge_array);
}

template <typename EdgeDataT>
inline void
read(storage::tar::FileReader &reader, const std::string &name, DynamicGraph<EdgeDataT> &graph)
{
    storage::serialization::read(reader, name + "/node_array", graph.node_array);
    const auto num_edges = reader.ReadElementCount64(name + "/edge_list");
    graph.edge_list.resize(num_edges);
    reader.ReadStreaming<typename std::remove_reference_t<decltype(graph)>::Edge>(
        name + "/edge_list", graph.edge_list.begin());
    graph.number_of_nodes = graph.node_array.size();
    graph.number_of_edges = num_edges;
}

template <typename EdgeDataT>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const DynamicGraph<EdgeDataT> &graph)
{
    storage::serialization::write(writer, name + "/node_array", graph.node_array);
    writer.WriteElementCount64(name + "/edge_list", graph.number_of_edges);
    writer.WriteStreaming<typename std::remove_reference_t<decltype(graph)>::Edge>(
        name + "/edge_list", graph.edge_list.begin(), graph.number_of_edges);
}

template <typename BlockPolicy, storage::Ownership Ownership>
inline void
read(storage::tar::FileReader &reader, const std::string &name, detail::IndexedDataImpl<BlockPolicy, Ownership> &index_data)
{
    storage::serialization::read(reader, name + "/blocks", index_data.blocks);
    storage::serialization::read(reader, name + "/values", index_data.values);
}

template <typename BlockPolicy, storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name, const detail::IndexedDataImpl<BlockPolicy, Ownership> &index_data)
{
    storage::serialization::write(writer, name + "/blocks", index_data.blocks);
    storage::serialization::write(writer, name + "/values", index_data.values);
}
}
}
}

#endif
