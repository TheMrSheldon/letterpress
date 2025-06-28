import standard
doctype Book {
	papersize: a4,
	fontsize: 12,
	title: "White Fang",
	author: [ "Jack London" ],
	date: "\today"
}
==
[preface>]
\showtitle
\showtoc
[<preface]

\part{}
\include{chapter1.1.lp}
\include{chapter1.2.lp}
