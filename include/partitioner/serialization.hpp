#ifndef OSRM_PARTITIONER_SERIALIZATION_HPP
#define OSRM_PARTITIONER_SERIALIZATION_HPP

#include "partitioner/cell_storage.hpp"
#include "partitioner/edge_based_graph.hpp"
#include "partitioner/multi_level_graph.hpp"
#include "partitioner/multi_level_partition.hpp"

#include "storage/block.hpp"
#include "storage/io.hpp"
#include "storage/serialization.hpp"
#include "storage/shared_memory_ownership.hpp"
#include "storage/tar.hpp"

namespace osrm
{
namespace partitioner
{
namespace serialization
{

template <typename EdgeDataT, storage::Ownership Ownership>
inline void read(storage::tar::FileReader &reader,
                 const std::string &name,
                 MultiLevelGraph<EdgeDataT, Ownership> &graph,
                 std::uint32_t &connectivity_checksum)
{
    storage::serialization::read(reader, name + "/node_array", graph.node_array);
    storage::serialization::read(reader, name + "/edge_array", graph.edge_array);
    storage::serialization::read(reader, name + "/node_to_edge_offset", graph.node_to_edge_offset);
    reader.ReadInto(name + "/connectivity_checksum", connectivity_checksum);
}

template <typename EdgeDataT, storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const MultiLevelGraph<EdgeDataT, Ownership> &graph,
                  const std::uint32_t connectivity_checksum)
{
    storage::serialization::write(writer, name + "/node_array", graph.node_array);
    storage::serialization::write(writer, name + "/edge_array", graph.edge_array);
    storage::serialization::write(writer, name + "/node_to_edge_offset", graph.node_to_edge_offset);
    writer.WriteElementCount64(name + "/connectivity_checksum", 1);
    writer.WriteFrom(name + "/connectivity_checksum", connectivity_checksum);
}

template <storage::Ownership Ownership>
inline void read(storage::tar::FileReader &reader,
                 const std::string &name,
                 detail::MultiLevelPartitionImpl<Ownership> &mlp)
{
    reader.ReadInto(name + "/level_data", *mlp.level_data);
    storage::serialization::read(reader, name + "/partition", mlp.partition);
    storage::serialization::read(reader, name + "/cell_to_children", mlp.cell_to_children);
}

template <storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const detail::MultiLevelPartitionImpl<Ownership> &mlp)
{
    writer.WriteElementCount64(name + "/level_data", 1);
    writer.WriteFrom(name + "/level_data", *mlp.level_data);
    storage::serialization::write(writer, name + "/partition", mlp.partition);
    storage::serialization::write(writer, name + "/cell_to_children", mlp.cell_to_children);
}

template <storage::Ownership Ownership>
inline void read(storage::tar::FileReader &reader,
                 const std::string &name,
                 detail::CellStorageImpl<Ownership> &storage)
{
    storage::serialization::read(reader, name + "/source_boundary", storage.source_boundary);
    storage::serialization::read(
        reader, name + "/destination_boundary", storage.destination_boundary);
    storage::serialization::read(reader, name + "/cells", storage.cells);
    storage::serialization::read(
        reader, name + "/level_to_cell_offset", storage.level_to_cell_offset);
}

template <storage::Ownership Ownership>
inline void write(storage::tar::FileWriter &writer,
                  const std::string &name,
                  const detail::CellStorageImpl<Ownership> &storage)
{
    storage::serialization::write(writer, name + "/source_boundary", storage.source_boundary);
    storage::serialization::write(
        writer, name + "/destination_boundary", storage.destination_boundary);
    storage::serialization::write(writer, name + "/cells", storage.cells);
    storage::serialization::write(
        writer, name + "/level_to_cell_offset", storage.level_to_cell_offset);
}
}
}
}

#endif
