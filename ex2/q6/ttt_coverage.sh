# run error
./ttt >/dev/null
./ttt "12345678" >/dev/null
./ttt "123456788" >/dev/null
./ttt "a" >/dev/null

# user input error
./ttt "123456789" <ttt_inputs/empty_input.txt >/dev/null
./ttt "123456789" <ttt_inputs/invalid_input1.txt >/dev/null
./ttt "123456789" <ttt_inputs/invalid_input2.txt >/dev/null
./ttt "123456789" <ttt_inputs/taken_cell.txt >/dev/null

#user win
./ttt "123456789" <ttt_inputs/user_win_col.txt >/dev/null
./ttt "123456789" <ttt_inputs/user_win_row.txt >/dev/null
./ttt "234567891" <ttt_inputs/user_win_main_diag.txt >/dev/null
./ttt "123456789" <ttt_inputs/user_win_sec_diag.txt >/dev/null

#computer win
./ttt "123456789" <ttt_inputs/computer_win.txt >/dev/null

#draw
./ttt "123456789" <ttt_inputs/draw.txt >/dev/null

gcov ttt.c # print coverage and create .gcov file
