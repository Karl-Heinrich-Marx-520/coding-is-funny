class Solution {
public:
    int minimumRefill(vector<int>& plants, int capacityA, int capacityB) {
        int n = plants.size(), right = n - 1, left = 0, ans = 0;
        int a = capacityA, b = capacityB;
        while(left <= right){
            if(left == right){
                if(max(a, b) < plants[left]) ans++;
                break;
            }
            
            if(a >= plants[left]) a -= plants[left++];
            else {
                ans++;
                a = capacityA - plants[left++];
            }
            
            if(b >= plants[right]) b -= plants[right--];
            else {
                ans++;
                b = capacityB - plants[right--];
            }
        }
        
        return ans;
    }
};
