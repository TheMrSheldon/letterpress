import standard
doctype Essay {
	papersize: a4,
	fontsize: 12,
	title: "Boxes, Penalty, and Glue",
	author: [ "Jane Doe", "John Everyman" ],
	date: "\today"
}
==
\writeString{Hello}\writeGlue{1}{2}{3}\writeString{World}