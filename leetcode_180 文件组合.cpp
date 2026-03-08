class Solution {
public:
    vector<vector<int>> fileCombination(int target) {
        vector<vector<int>> ans;
        int sum = 0, left = 1;
        for(int r = 1; r < target; ++r){
            sum += r;
            
            while(sum > target){
                sum -= left;
                left++;
            }

            if(sum == target){
                vector<int> element;
                for(int i = left; i <= r; ++i){
                    element.emplace_back(i);
                }
                ans.emplace_back(element);
            }
        }
        return ans;
    }
};
