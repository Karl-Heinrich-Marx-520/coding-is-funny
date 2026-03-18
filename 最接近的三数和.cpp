class Solution {
public:
    int threeSumClosest(std::vector<int>& nums, int target) {
        ranges::sort(nums);
        int ans = nums[0] + nums[1] + nums[2], n = nums.size();
        for(int p = 0; p < n; ++p){
            int left = p+1, right = n-1;
            while(left < right){
                int sum = nums[p] + nums[left] + nums[right];
                if(abs(target - sum) < abs(target - ans)) ans = sum;
                if(sum > target) right--;
                else if(sum < target) left++;
                else return ans;
            }
        }
        return ans;
    }
};
