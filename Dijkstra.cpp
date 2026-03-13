#include <iostream>
#include <vector>
#include <queue>
#include <climits>
using namespace std;

const int INF = INT_MAX;

/**
 * 堆优化版迪杰斯特拉算法
 * @param adj 邻接表，adj[u] 存储 {v, 边权} 对
 * @param start 源点编号
 * @param n 节点总数
 * @return 源点到所有节点的最短距离数组
 */
vector<int> dijkstra_heap(const vector<vector<pair<int, int>>>& adj, int start, int n) {
    vector<int> dist(n, INF);
    dist[start] = 0;

    // 小根堆：存储 {当前距离, 节点编号}，优先弹出距离最小的节点
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> min_heap;
    min_heap.emplace(0, start);

    while (!min_heap.empty()) {
        auto [current_dist, u] = min_heap.top();
        min_heap.pop();

        // 如果弹出的距离已大于记录的最短距离，说明是旧数据，跳过
        if (current_dist > dist[u]) continue;

        // 遍历 u 的所有邻接节点，执行松弛操作
        for (auto [v, weight] : adj[u]) {
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                min_heap.emplace(dist[v], v);
            }
        }
    }

    return dist;
}
