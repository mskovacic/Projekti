#include <vector>

class Board {
public:
	Board(int size);
	int size();
	void place(int row, int col);
	int unplace(int row);
	bool is_ok(int row, int col);
	bool solve();
	int col(int row);

private:

	int size_;
	std::vector<int> pos_;
	std::vector<bool> col_;
	std::vector<bool> diag1_;
	std::vector<bool> diag2_;
};
