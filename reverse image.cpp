class Solution {
public:
    vector<vector<int>> flipAndInvertImage(vector<vector<int>>& image) {
        for(auto& pel : image){
            ranges::reverse(pel);
            
            for(auto& in : pel){
                if(in == 1) in = 0;
                else in = 1;
            }
        }
        return image;
    }
};
