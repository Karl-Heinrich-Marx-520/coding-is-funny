class Solution {
public:
    int triangleNumber(vector<int>& nums) {
        ranges::sort(nums);
        int ans = 0, n = nums.size();
        for(int p = 0; p < n-2; ++p){
            if(nums[p] == 0) continue;
            int target = nums[p], left = p+1;
            
           for(int right = p+2; right < n; ++right){
               while(nums[right] - nums[left] >= target) left++;
               ans += right - left;
               }
            }
        return ans;
    }
};
// 2 3 4 4
