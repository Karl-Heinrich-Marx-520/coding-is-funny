class Solution {
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) {
        vector<vector<int>> ans;
        ranges::sort(nums);
        int n = nums.size();
        for(int p1 = 0; p1 < n-3; ++p1){
            if(p1 && nums[p1] == nums[p1-1]) continue;

            for(int p2 = p1+1; p2 < n-2; ++p2){
                if(p2-1 != p1 && nums[p2] == nums[p2-1]) continue;

                int left = p2+1, right = n-1;
                while(left < right){
                    long long sum = (long long)nums[p1] + nums[p2] + nums[left] + nums[right];
                    if(sum < target) left++;
                    else if(sum > target) right--;
                    else {
                        ans.push_back({nums[p1], nums[p2], nums[left], nums[right]});
                        left++; right--;
                        while(left < right && nums[left] == nums[left-1]) left++;
                        while(left < right && nums[right] == nums[right+1]) right--;
                    }
                }
            }
        }
        return ans;
    }
};

// -2 -1 0 0 1 2
