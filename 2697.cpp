class Solution {
public:
    string makeSmallestPalindrome(string s) {
        int right = s.size() - 1, left = 0;
        while(right > left){
            if(s[right] != s[left]){
                if(s[right] > s[left]) s[right] = s[left];
                else s[left] = s[right];
            }
            left++;
            right--;
        }
        return s;
    }
};
