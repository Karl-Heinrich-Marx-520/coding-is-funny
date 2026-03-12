class Solution {
public:
    vector<int> sortedSquares(vector<int>& nums) {
        int n = nums.size(), left = 0, right = n - 1;
        vector<int> ans(n);
        for(int p = n-1; p >= 0; --p){
            int x = nums[left] * nums[left];
            int y = nums[right] * nums[right];
            
            if(x > y) {
                ans[p] = x;
                left++;
            }
            else {
                ans[p] = y;
                right--;
            }
        }
        
        return ans;
    }
};
