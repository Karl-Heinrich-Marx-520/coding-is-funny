#include <iostream>
using namespace std;

// 核心：堆化函数，把以i为根的子树变成大顶堆
void heapify(int arr[], int n, int i) {
    int largest = i;        // 假设当前节点最大
    int left = 2 * i + 1;    // 左孩子
    int right = 2 * i + 2;   // 右孩子

    // 左孩子更大
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // 右孩子更大
    if (right < n && arr[right] > arr[largest])
        largest = right;

    // 最大值不是自己，交换并继续堆化
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// 堆排序主函数
void heapSort(int arr[], int n) {
    // 1. 构建大顶堆：从最后一个非叶子节点往上
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // 2. 一个个从堆顶取出元素
    for (int i = n - 1; i > 0; i--) {
        // 堆顶和当前最后一个交换
        swap(arr[0], arr[i]);

        // 堆长度减小，重新堆化堆顶
        heapify(arr, i, 0);
    }
}

int main() {
    int arr[] = {4, 6, 8, 5, 9};
    int n = sizeof(arr)/sizeof(arr[0]);

    heapSort(arr, n);

    cout << "排序后：";
    for(int i=0; i<n; i++) cout << arr[i] << " ";
    return 0;
}
