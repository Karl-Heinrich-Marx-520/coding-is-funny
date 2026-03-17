class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> ans;
        ranges::sort(nums);
        int n = nums.size();
        
        for(int p = 0; p < n-2; ++p){
            while(p < n-2 && p && nums[p] == nums[p-1]) p++;
            int left = p+1, right = n-1, target = -nums[p];
            
            while(left < right){
                
                 while(left < right && nums[left] +nums[right] != target){
                    while(left < right && nums[left] + nums[right] > target) right--;
                    while(left < right && nums[left] + nums[right] < target) left++;
                 }
                 
                while(left+1 < right && nums[left] == nums[left+1]) left++;
                while(left+1 < right && nums[right] == nums[right-1]) right--;
                
                if(left < right && nums[left] + nums[right] == target){
                    ans.push_back({nums[p], nums[left], nums[right]});
                    left++;
                    right--;
                }     
            }// 0 0 0 0
        }
        return ans;
    }
};
//-4 -4 -4 -4 -3 -3 -2 -1 0 2 2 2 3 3
