#include <BhaalBot.hpp>
#include <BWAPIUtil.hpp>
#include <DangerZones.hpp>
#include <Unit.hpp>
#include <Player.hpp>
#include <algorithm>
#include <UnitAsAttackTargetEvaulation.hpp>

DangerZones::DangerZones(ModuleContainer& moduleContainer)
  : Module(moduleContainer)
{}

void DangerZones::onStart()
{
  this->width = BWAPI::Broodwar->mapWidth();
  this->height = BWAPI::Broodwar->mapHeight();
  this->data.resize(this->width);
  for (int32_t x = 0; x < this->width; ++x)
    this->data[x].resize(this->height);
}

void DangerZones::onFrame()
{
  for (int32_t x = 0; x < this->width; ++x)
    for (int32_t y = 0; y < this->height; ++y)
      if (this->data[x][y].danger != 0)
        BWAPI::Broodwar->drawTextMap(BWAPI::Position(BWAPI::TilePosition(x, y)), "%d", this->data[x][y]);
}

void DangerZones::addDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value)
{
  ++radius;
  BWAPI::TilePosition centerPositionTiled(centerPosition);
  BWAPI::TilePosition position;
  for (position.x = std::max(0, centerPositionTiled.x - radius); position.x < std::min(this->width, centerPositionTiled.x + radius); ++position.x)
    for (position.y = std::max(0, centerPositionTiled.y - radius); position.y < std::min(this->width, centerPositionTiled.y + radius); ++position.y)
      if (position.getApproxDistance(centerPositionTiled) <= radius)
        this->data[position.x][position.y].danger += value;
}

void DangerZones::removeDanger(BWAPI::Position centerPosition, int32_t radius, int32_t value)
{
  ++radius;
  BWAPI::TilePosition centerPositionTiled(centerPosition);
  BWAPI::TilePosition position;
  for (position.x = std::max(0, centerPositionTiled.x - radius); position.x < std::min(this->width, centerPositionTiled.x + radius); ++position.x)
    for (position.y = std::max(0, centerPositionTiled.y - radius); position.y < std::min(this->width, centerPositionTiled.y + radius); ++position.y)
      if (position.getApproxDistance(centerPositionTiled) <= radius)
        this->data[position.x][position.y].danger -= value;
}

class WaveNode
{
public:
  WaveNode(BWAPI::TilePosition position, double distanceFromStart)
    : position(position)
    , distanceFromStart(distanceFromStart) {}
  bool operator<(const WaveNode& other) const { return this->distanceFromStart > other.distanceFromStart; }

  BWAPI::TilePosition position;
  double distanceFromStart;
};

TargetWithPath DangerZones::findBestTarget(BWAPI::Position startingPosition)
{
  static const double dangerValue = -1;
  static const double unitValue = 1000;
  // TODO plz comment what is the algo
  std::vector<WaveNode> wave;
  BWAPI::TilePosition startingPositionTiled = BWAPI::TilePosition(startingPosition);
  Node& startingNode = this->data[startingPositionTiled.x][startingPositionTiled.y];
  startingNode.distanceFromStart = 1;
  wave.push_back(WaveNode(startingPositionTiled, 1));
  std::make_heap (wave.begin(),wave.end());
  int width = BWAPI::Broodwar->mapWidth();
  int height = BWAPI::Broodwar->mapHeight();

  Unit* bestCandidate = nullptr;
  double bestCandidateScore = 0;

  while (!wave.empty())
  {
    std::pop_heap(wave.begin(), wave.end());
    WaveNode node = wave.back();
    Node& thisNode = this->data[node.position.x][node.position.y];
    wave.pop_back();
    for (Direction direction: Direction::allDirections)
    {
      BWAPI::TilePosition candidatePosition = node.position;
      BWAPIUtil::moveByOne(&candidatePosition, direction);
      if (candidatePosition.x < 0 ||
          candidatePosition.y < 0 ||
          candidatePosition.x >= width ||
          candidatePosition.y >= height)
        continue;
      Node& candidate = this->data[candidatePosition.x][candidatePosition.y];
      if (candidate.distanceFromStart != 0)
        continue;
      double addedDanger = thisNode.danger + candidate.danger + 0.1;
      if (direction.isDiagonal())
        addedDanger *= M_SQRT2;
      candidate.distanceFromStart = thisNode.distanceFromStart + addedDanger;
      candidate.cameFrom = direction.opposite();
      for (Unit* unit = bhaalBot->units.getTile(candidatePosition).units; unit != nullptr; unit = unit->nextOnTile)
        if (unit->getPlayer()->isEnemy)
        {
          double score = UnitAsAttackTargetEvaluation::value(unit) * unitValue +
                         thisNode.distanceFromStart * dangerValue;
          if (bestCandidate == nullptr ||
              bestCandidateScore < score)
          {
            bestCandidate = unit;
            bestCandidateScore = score;
          }
        }
      wave.push_back(WaveNode(candidatePosition, candidate.distanceFromStart));
      std::push_heap(wave.begin(), wave.end());
    }
  }
  this->clearCostFromStart();
  TargetWithPath result;
  if (bestCandidate == nullptr)
    return result;
  result.target = bestCandidate;
  BWAPI::TilePosition currentPosition = BWAPI::TilePosition(bestCandidate->getPosition());
  while (currentPosition != startingPositionTiled)
  {
    result.path.push_back(currentPosition);
    BWAPIUtil::moveByOne(&currentPosition, this->data[currentPosition.x][currentPosition.y].cameFrom);
  }
  return result;
}

void DangerZones::clearCostFromStart()
{
  int width = BWAPI::Broodwar->mapWidth();
  int height = BWAPI::Broodwar->mapHeight();
  for (auto& row: this->data)
    for (Node& node: row)
      node.distanceFromStart = 0;
}
