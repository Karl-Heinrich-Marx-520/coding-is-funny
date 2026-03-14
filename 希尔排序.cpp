#include <iostream>
using namespace std;

void shellSort(int arr[], int n) {
    // 控制增量 gap，不断缩小
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 从 gap 开始，对每个分组做插入排序
        for (int i = gap; i < n; ++i) {
            int key = arr[i];
            int j;
            // 组内插入排序
            for (j = i; j >= gap && arr[j - gap] > key; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = key;
        }
    }
}

int main() {
    int arr[] = {8,9,1,7,2,3,5,4,6,0};
    int n = sizeof(arr)/sizeof(arr[0]);

    shellSort(arr, n);

    for(int x : arr) cout << x << " ";
    return 0;
}
