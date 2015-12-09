#include <functional>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

using std::cin;
using std::cout;

void merge_sort(std::vector<int> &arr, int l, int r, std::function<bool(int, int)> comp)
{
	if(r - l == 1)
		return;
	int m = (l + r) / 2;
	merge_sort(arr, l, m, comp);
	merge_sort(arr, m, r, comp);
	int merged_arr[r - l];
	int out_p = 0, left_p = l, right_p = m;
	
	while(left_p < m && right_p < r)
		if(comp(arr[left_p], arr[right_p]))
			merged_arr[out_p++] = arr[left_p++];
		else
			merged_arr[out_p++] = arr[right_p++];
	
	while(left_p < m)
		merged_arr[out_p++] = arr[left_p++];
	while(right_p < r)
		merged_arr[out_p++] = arr[right_p++];
		
	for(int i = 0; i < r - l; i++)
		arr[l + i] = merged_arr[i];
}

int main()
{
	int n;
	cin >> n;
	std::vector<int> input(n);
	copy_n(std::istream_iterator<int>(cin), n, begin(input));
	
	auto comp_less = [](int a, int b){return a < b;};
	auto comp_greater = [](int a, int b){return a > b;};
	
	merge_sort(input, 0, n, comp_less);
	cout << "Ascending order:\n";
	for(auto it: input)
		cout << it << ' ';
	cout << "\n";
	merge_sort(input, 0, n, comp_greater);
	cout << "Descending order:\n";
	for(auto it: input)
		cout << it << ' ';
	cout << "\n";
}
