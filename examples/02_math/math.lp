doctype essay {
	papersize: a4,
	fontsize: 12,
	title: TestDocument,
	author: [Jane Doe | John Everyman],
	institute: [Institute A | Institute B],
	date: \today,
}

import liba
import libb
====
\maketitle

Hello World import 321d

test  test \command{hello}{world}

Demo of math mode: $x_{1,2} = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}$
and in display mode:
$$x_{1,2} = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}$$

Typing \$ as part of a word: \$xyz\$test\$

[table>]
	Column 1 | Column 2 | Column 3\\
	Row 1    | Row 2    | Row 3\\
[<table]

[picture>]
	
[<picture]