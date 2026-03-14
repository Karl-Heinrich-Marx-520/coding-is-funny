class Solution {
public:
    vector<int> getStrongest(vector<int>& arr, int k) {
        ranges::sort(arr);
        int n = arr.size(), left = 0, right = n-1, mid = arr[(n-1)/2];
        vector<int> ans(k, 0);
        for(int p = 0; p < k; ++p){
            int num_r = abs(arr[right] - mid);
            int num_l = abs(arr[left] - mid);

            if(num_r >= num_l) ans[p] = arr[right--];
            else ans[p] = arr[left++];
        }

        return ans;
    }
};
