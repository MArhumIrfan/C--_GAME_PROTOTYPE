#include "Enemy.h"
#include "Constants.h"
#include "Player.h"
#include <cmath>
#include <queue>
#include <vector>
#include <algorithm>

void Enemy::update(float dt, const Player& player, const MapCell worldMap[MAP_H][MAP_H]) { // <-- CORRECTED
    if (!active) return;

    bool playerInCrawlspace = worldMap[(int)player.posY][(int)player.posX].wallType == 3;
    float distToPlayer = std::hypot(player.posX - x, player.posY - y);
    animTimer += dt;
    if (animTimer > 0.35f) {
        currentFrame = 1 - currentFrame;
        animTimer = 0.0f;
    }

    switch (state) {
        case AI_STATE_IDLE:
            isChasing = false;
            pathRecalculateTimer -= dt;
            if (pathRecalculateTimer <= 0.0f) {
                int targetX = rand() % MAP_W;
                int targetY = rand() % MAP_H;
                if (worldMap[targetY][targetX].wallType == 0) {
                    currentPath = findPath({(int)x, (int)y}, {targetX, targetY}, worldMap);
                }
                pathRecalculateTimer = 5.0f + (rand() % 5);
            }
            if (!currentPath.empty()) {
                Point nextWaypoint = currentPath.front();
                moveToward(nextWaypoint.x + 0.5f, nextWaypoint.y + 0.5f, dt);
                if (std::hypot(x - (nextWaypoint.x + 0.5f), y - (nextWaypoint.y + 0.5f)) < 0.5f) {
                    currentPath.erase(currentPath.begin());
                }
            }
            break;

        case AI_STATE_WATCHING:
            isChasing = false;
            speed = STALKER_WATCHER_SPEED;
            if (!playerInCrawlspace && distToPlayer < 6.0f) {
                state = AI_STATE_LUNGING;
                lungeTimer = 1.5f;
            } else if (distToPlayer < 10.0f) {
                moveToward(x - (player.posX - x), y - (player.posY - y), dt);
            }
            break;

        case AI_STATE_LUNGING:
            isChasing = true;
            lungeTimer -= dt;
            speed = STALKER_LUNGE_SPEED;
            moveToward(player.posX, player.posY, dt);
            if (lungeTimer <= 0.0f || distToPlayer > 7.0f) {
                state = AI_STATE_WATCHING;
            }
            break;

        case AI_STATE_HUNTING:
            if (playerInCrawlspace) {
                isChasing = false;
                currentPath.clear();
                break;
            }
            isChasing = true;
            speed = STALKER_HUNTER_SPEED + (player.noiseLevel * 0.5f);
            pathRecalculateTimer -= dt;
            if (pathRecalculateTimer <= 0.0f) {
                currentPath = findPath({(int)x, (int)y}, {(int)player.posX, (int)player.posY}, worldMap);
                pathRecalculateTimer = 1.5f - player.noiseLevel;
            }
            if (!currentPath.empty()) {
                Point nextWaypoint = currentPath.front();
                moveToward(nextWaypoint.x + 0.5f, nextWaypoint.y + 0.5f, dt);
                if (std::hypot(x - (nextWaypoint.x + 0.5f), y - (nextWaypoint.y + 0.5f)) < 0.5f) {
                    currentPath.erase(currentPath.begin());
                }
            }
            break;

        case AI_STATE_INVESTIGATING:
             isChasing = false;
            speed = STALKER_WATCHER_SPEED;
            investigateTimer -= dt;
            if (investigateTimer <= 0.0f || currentPath.empty()) {
                state = AI_STATE_HUNTING;
            } else {
                 Point nextWaypoint = currentPath.front();
                moveToward(nextWaypoint.x + 0.5f, nextWaypoint.y + 0.5f, dt);
                if (std::hypot(x - (nextWaypoint.x + 0.5f), y - (nextWaypoint.y + 0.5f)) < 0.5f) {
                    currentPath.erase(currentPath.begin());
                }
            }
            break;
    }
}

void Enemy::reset(bool setActive, AIState startState) {
    x = MAP_W / 2.0f + 0.5f;
    y = MAP_H / 2.0f + 0.5f;
    active = setActive;
    state = startState;
    isChasing = false;
    currentPath.clear();
    pathRecalculateTimer = 0.0f;
}

void Enemy::setInvestigateTarget(float targetX, float targetY, const MapCell worldMap[MAP_W][MAP_H]) { // <-- CORRECTED
    if(state == AI_STATE_HUNTING || state == AI_STATE_INVESTIGATING) {
        state = AI_STATE_INVESTIGATING;
        investigateTarget = {(int)targetX, (int)targetY};
        currentPath = findPath({(int)x, (int)y}, investigateTarget, worldMap);
        investigateTimer = 8.0f;
    }
}

void Enemy::moveToward(float targetX, float targetY, float dtSec) {
    float dist = std::hypot(targetX - x, targetY - y);
    if (dist < 0.1f) return;
    float dx = (targetX - x) / dist;
    float dy = (targetY - y) / dist;
    x += dx * speed * dtSec;
    y += dy * speed * dtSec;
}

std::vector<Point> Enemy::findPath(Point start, Point end, const MapCell worldMap[MAP_H][MAP_W]) { // <-- CORRECTED
    std::vector<Point> path;
    struct AStarNode {
        int x, y, g, h;
        AStarNode* parent;
        int f() const { return g + h; }
    };

    auto cmp = [](const AStarNode* a, const AStarNode* b) { return a->f() > b->f(); };
    std::priority_queue<AStarNode*, std::vector<AStarNode*>, decltype(cmp)> openSet(cmp);
    std::vector<AStarNode*> allNodes;

    AStarNode* startNode = new AStarNode{start.x, start.y, 0, std::abs(start.x - end.x) + std::abs(start.y - end.y), nullptr};
    openSet.push(startNode);
    allNodes.push_back(startNode);
    bool inOpenSet[MAP_H][MAP_W] = {false};
    inOpenSet[start.y][start.x] = true;
    bool closedSet[MAP_H][MAP_W] = {false};

    while (!openSet.empty()) {
        AStarNode* current = openSet.top();
        openSet.pop();

        if (current->x == end.x && current->y == end.y) {
            while (current != nullptr) {
                path.push_back({current->x, current->y});
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());
            break;
        }

        closedSet[current->y][current->x] = true;
        const int dx[] = {0, 0, 1, -1};
        const int dy[] = {1, -1, 0, 0};

        for (int i = 0; i < 4; ++i) {
            int nx = current->x + dx[i], ny = current->y + dy[i];
            if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H || (worldMap[ny][nx].wallType != 0 && worldMap[ny][nx].wallType != 6) || closedSet[ny][nx]) {
                continue;
            }
            if (!inOpenSet[ny][nx]) {
                AStarNode* neighbor = new AStarNode{nx, ny, current->g + 1, std::abs(nx - end.x) + std::abs(ny - end.y), current};
                openSet.push(neighbor);
                allNodes.push_back(neighbor);
                inOpenSet[ny][nx] = true;
            }
        }
    }
    for (auto node : allNodes) delete node;
    return path;
}