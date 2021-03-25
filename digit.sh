#!/bin/sh

mongo_0() {
	printf '%s\n' \
	       '    [XXXX]      ' \
	       '  [XX]  [XX]    ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]  []  [XX]  ' \
	       '[XX]  []  [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XX]  [XX]    ' \
	       '    [XXXX]      '
}

mongo_1() {
	printf '%s\n' \
	       '      [XX]      ' \
	       '    [XXXX]      ' \
	       '  [XXXXXX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '  [XXXXXXXXXX]  '
}

mongo_2() {
	printf '%s\n' \
	       '  [XXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '        [XX]    ' \
	       '      [XX]      ' \
	       '    [XX]        ' \
	       '  [XX]          ' \
	       '[XX]            ' \
	       '[XX]      [XX]  ' \
	       '[XXXXXXXXXXXX]  '
}

mongo_3() {
	printf '%s\n' \
	       '  [XXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '    [XXXXXX]    ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXX]    '
}

mongo_4() {
	printf '%s\n' \
	       '        [XX]    ' \
	       '      [XXXX]    ' \
	       '    [XXXXXX]    ' \
	       '  [XX]  [XX]    ' \
	       '[XX]    [XX]    ' \
	       '[XXXXXXXXXXXX]  ' \
	       '        [XX]    ' \
	       '        [XX]    ' \
	       '        [XX]    ' \
	       '        [XX]    ' \
	       '        [XX]    ' \
	       '      [XXXXXX]  '
}

mongo_5() {
	printf '%s\n' \
	       '[XXXXXXXXXXXX]  ' \
	       '[XX]            ' \
	       '[XX]            ' \
	       '[XX]            ' \
	       '[XX]            ' \
	       '[XXXXXXXXXX]    ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXX]    '
}

mongo_6() {
	printf '%s\n' \
	       '      [XXXX]    ' \
	       '    [XX]        ' \
	       '  [XX]          ' \
	       '[XX]            ' \
	       '[XX]            ' \
	       '[XXXXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXX]    '
}

mongo_7() {
	printf '%s\n' \
	       '[XXXXXXXXXXXX]  ' \
	       '[XX]      [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '        [XX]    ' \
	       '      [XX]      ' \
	       '    [XX]        ' \
	       '    [XX]        ' \
	       '    [XX]        ' \
	       '    [XX]        ' \
	       '    [XX]        ' \
	       '    [XX]        '
}

mongo_8() {
	printf '%s\n' \
	       '  [XXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXX]    '
}

mongo_9() {
	printf '%s\n' \
	       '  [XXXXXXXX]    ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '[XX]      [XX]  ' \
	       '  [XXXXXXXXXX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '          [XX]  ' \
	       '        [XX]    ' \
	       '      [XX]      ' \
	       '  [XXXX]        '
}

mongo_c() {
	printf '%s\n' \
	       '                ' \
	       '                ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '                ' \
	       '                ' \
	       '                ' \
	       '                ' \
	       '      [XX]      ' \
	       '      [XX]      ' \
	       '                ' \
	       '                '
}

digit() {
	if test "$1" = 0; then
		mongo_0
	elif test "$1" = 1; then
		mongo_1
	elif test "$1" = 2; then
		mongo_2
	elif test "$1" = 3; then
		mongo_3
	elif test "$1" = 4; then
		mongo_4
	elif test "$1" = 5; then
		mongo_5
	elif test "$1" = 6; then
		mongo_6
	elif test "$1" = 7; then
		mongo_7
	elif test "$1" = 8; then
		mongo_8
	elif test "$1" = 9; then
		mongo_9
	else
		mongo_c
	fi
}

printf 'static const char *mongo_%s[] = {\n' "$1"
digit "$1" | \
    sed -e 's/X/ /g' -e 's/\[/\\033\[7m /g' -e 's/\]/ \\033\[m/g' | \
    sed -e 's/^/"/' -e 's/$/",/' | \
    sed '$s/,$//'
printf '};\n'
