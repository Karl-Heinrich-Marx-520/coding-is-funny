lass Solution {
public:
    int purchasePlans(vector<int>& nums, int target) {
        const long long MOD = 1e9 + 7;
        ranges::sort(nums);
        int n = nums.size(), left = 0, right = n-1, ans = 0;
        while(left < right){
            if(nums[left] + nums[right] <= target){
                ans += right - left;
                ans %= MOD;
                left++;
            }
            else right--;
        }

        return ans % MOD;
    }
};

// 1 2 2 9
