class Solution {
public:
    int minimumLength(string s) {
        int n = s.size(), left = 0, right = n - 1;
        if(n == 1) return 1;
        while(left < right && s[left] == s[right]){
            while(right >= 0 && s[left] == s[right]) right--;
            while(left < n && s[left] == s[right+1]) left++;
        }
        
        return max(0, right - left + 1);
    }
};
