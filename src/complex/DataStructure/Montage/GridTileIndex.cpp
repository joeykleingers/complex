#include "GridTileIndex.hpp"

#include "complex/DataStructure/Montage/GridMontage.hpp"

using namespace complex;

GridTileIndex::GridTileIndex()
: AbstractTileIndex()
{
}

GridTileIndex::GridTileIndex(const GridMontage* montage, const SizeVec3& pos)
: AbstractTileIndex(montage)
, m_Pos(pos)
{
}

GridTileIndex::GridTileIndex(const GridTileIndex& other)
: AbstractTileIndex(other)
, m_Pos(other.m_Pos)
{
}

GridTileIndex::GridTileIndex(GridTileIndex&& other) noexcept
: AbstractTileIndex(std::move(other))
, m_Pos(std::move(other.m_Pos))
{
}

GridTileIndex::~GridTileIndex() = default;

size_t GridTileIndex::getRow() const
{
  return m_Pos[1];
}

size_t GridTileIndex::getCol() const
{
  return m_Pos[0];
}

size_t GridTileIndex::getDepth() const
{
  return m_Pos[2];
}

SizeVec3 GridTileIndex::getTilePos() const
{
  return m_Pos;
}

const AbstractGeometry* GridTileIndex::getGeometry() const
{
  auto montage = dynamic_cast<const GridMontage*>(getMontage());
  if(!montage)
  {
    return nullptr;
  }
  return montage->getGeometry(this);
}

TooltipGenerator GridTileIndex::getToolTipGenerator() const
{
  throw std::exception();
}

bool GridTileIndex::isValid() const
{
  if(!dynamic_cast<const GridMontage*>(getMontage()))
  {
    return false;
  }
  return AbstractTileIndex::isValid();
}
