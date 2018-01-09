#include <iostream>
#include <random>
#include <deque>
#include <time.h>
#include <string>
#include <queue>
#include <windows.h>
#include <ctime>

using namespace std;

const int s = 16;

string goal_board = "123456789ABCDEF ";

int start_pos;

enum move_key { Up, Down, Left, Right };

deque<move_key> good_moves;

//============================================================State========================================================================
class State {

public:

	int empty_ind;

	string board = {'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F', ' '};

	int h;
	int g;
	int f;

	State() {
		h = 0;
		g = 0;
		f = 0;
	}

	State(int empty_i, string arr) : empty_ind(empty_i)
	{
		for (int i = 0; i < 16; i++)
		{
			board[i] = arr[i];
		}
		h = 0;
		g = 0;
		f = 0;
	}

	friend bool operator==(const State& s1, const State& s2) {
		return (s1.board == s2.board);
	}

	//=========================Filling with string=========================
	void fill_with_string(string s) {
		for (int i = 0; i < 16; i++)
		{
			board[i] = s[i];
		}
		empty_ind = s.find_first_of(' ');
	}

	//=========================Generate random board=========================
	void generate_board() {
		srand(time(NULL));
		for (int i = 0; i < 16; i++) {
			swap(board[rand() % 16], board[rand() % 16]);
		}
	}


	//=========================Print board=========================
	void print() {
		for (int i = 0; i < s; i++) {
			if (i % 4 == 0)
				cout << "\n_____________________ \n" << '|';

			if (board[i] >= '0' && board[i] <= '9')
				printf(" %-2d |", board[i] - '0');
			else
				if (board[i] >= 'A' && board[i] <= 'F')
					printf(" %-2d |", board[i] + 10 - 'A');
				else
					printf("    |");
		}
		cout << "\n_____________________ \n";
	}


	//=========================Checking is state is goal=========================
	bool is_goal() {
		for (int i = 0; i < s; i++) {
			if (board[i] != goal_board[i])
				return false;
		}
		return true;
	}

	//=========================Checking is move is valid=========================
	bool CanMove(move_key m) {
		switch (m) {
		case Up:
			return (empty_ind < 12);
			break;
		case Down:
			return (empty_ind > 3);
			break;
		case Left:
			return ((empty_ind+1) % 4); 
			break;
		case Right:
			return ((empty_ind) % 4);
			break;
		}
	}

  //=========================Making move=========================
  State Move(move_key m) {
		State new_state = State(empty_ind, board);

		//cout << " I'm in move!" << endl;
		if (CanMove(m))
		{
			switch (m) {
			case Up:
				swap(new_state.board[empty_ind], new_state.board[empty_ind + 4]);
				new_state.empty_ind += 4;
				break;
			case Down:
				swap(new_state.board[empty_ind], new_state.board[empty_ind - 4]);
				new_state.empty_ind -= 4;
				break;
			case Left:
				swap(new_state.board[empty_ind], new_state.board[empty_ind + 1]);
				new_state.empty_ind += 1;
				break;
			case Right:
				swap(new_state.board[empty_ind], new_state.board[empty_ind - 1]);
				new_state.empty_ind -= 1;
				break;
			}
			//cout << "Making move: " << m << endl;
			//cout << "H: " << h << endl;
			//print();
			//Sleep(1000);
		}
		else
			return *this;

		return new_state;
	}

   //=========================Distanace for each element=========================
	int distance(int a, int b) {
		int steps = 0;
		while (a != b) {
			if (a < b && a / 4 != b / 4) //different rows,down
			{
				a += 4;
				steps++;
			}
			if (a < b && a / 4 == b / 4) //same row, to the left
			{
				a++;
				steps++;
			}
			if (a > b && a / 4 != b / 4) //different rows, up
			{
				a -= 4;
				steps++;
			}
			if (a > b && a / 4 == b / 4) //same row, to the right
			{
				a--;
				steps++;
			}
		}
		return steps;
	}

	//=========================Countung manh distanace=========================
	int manh() {
		int m = 0;
		for (int i = 0; i < 16; i++) {
			if (goal_position(i) != i && board[i]!=' ')
				m += distance(i, goal_position(i));
		}
		this->h = m;
		//cout << "Manh distance:" << m << endl;
		//Sleep(1000);
		return m;
	}

	
	//=========================Finding goal position for a number
	int goal_position(int a) {
		return goal_board.find_first_of(board[a]);
	}

	//=========================If solvable=========================
	bool can_solve() {
		int sum = 1;
		for (int i = 0; i < 16; ++i)
		{
			for (int j = i + 1; j < 16; ++j)
				if ((board[i] > board[j]) && (board[j]))
					++sum;
		}
		sum += empty_ind / 4;
		return (!(sum % 2));
	}


};


//===========================================================================Solving functions


//=========================Deepening
int search(deque<State> & states, int bound) {

	State st = states.back();

	if (st.f > bound)
		return st.f;

	if (st.is_goal()) { 
		cout << "Solved! ";
		st.print();
		cout << "Total steps: " << st.g << endl;
		return -1;
	}

	int min_f = INT_MAX;

	State child;
	for (int i = 0; i < 4; i++)
	{
		move_key path = move_key(i);
		if (st.CanMove(path))
		{
			child = st.Move(path);

			child.g = st.g + 1;
			child.h = child.manh();
			child.f = child.g + child.h;

			if (child.f <= bound && (find(states.begin(), states.end(), child) == states.end())) 
			{
				states.push_back(child);

				int child_h = search(states, bound);

				if (child_h == -1)
					return -1; // found!

				states.pop_back(); //cutting off bad state if not found

				if (child_h < min_f && child_h > bound)
					min_f = child_h;
			}
			else
			if (child.f  < min_f && child.f  > bound)
			{
				min_f = child.f;
			}
		}
	}
	return min_f;
}

//=========================IDA* algorighm=========================
void ida_star(State state) {
	deque<State> states;

	int bound = state.manh();
	state.g = 0;
	//cout << "manh" << state.manh() << endl;

	state.f = state.manh();

	states.push_back(state);

	int res = 0;

	while (true)
	{
		//cout << "Bound: " << bound << endl;
		res = search(states, bound);
		if (res == -1)
		{
			//cout << states.size() << endl;
			return;
		}
		bound = res;
	}

}

//=========================Starting algorighm
void start_solving(State & s) {
	cout << "Initial board:";
	s.print();

	if (!s.can_solve())
	{
		cout << "This board can't be solved." << endl;
		return;
	}

	int start_time = clock();
	ida_star(s);
	int end_time = clock();
	cout << "Total time: " << (end_time - start_time) / (double)1000 << "sec." << endl;
}







int main() {
	State nb;
	//nb.generate_board();
	//nb.print();
	//nb.fill_with_string("75AB2C416D389F E");

	nb.fill_with_string("123456789A FDECB");
	//nb.print();
	//nb.fill_with_string("123456789ABCFE "); //can't solve
	//cout << nb.can_solve();
	//cout << nb.CanMove(Up) << endl;
	//cout << nb.CanMove(Down) << endl;
	//cout << nb.CanMove(Right) << endl;
	//cout << nb.CanMove(Left) << endl;
	//nb.start_count();
	start_solving(nb);
	//nb.fill_with_string("75AB2C416D389F E");
	//start_solving(nb);


	system("pause");
}