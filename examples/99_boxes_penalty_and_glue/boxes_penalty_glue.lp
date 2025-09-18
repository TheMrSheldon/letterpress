import standard
doctype Essay {
	papersize: a4,
	fontsize: 12,
	title: "Boxes, Penalty, and Glue",
	author: [ "Jane Doe", "John Everyman" ],
	date: "\today"
}
==

\paragraph{Test}

III

I\writeGlue{0}{0}{0}I\writeGlue{0}{0}{0}I

I\writeGlue{10}{0}{0}I\writeGlue{0}{0}{0}I

I\writeGlue{10}{0}{0}I\writeGlue{10}{0}{0}I

I\writeGlue{0}{100}{0}I\writeGlue{0}{10000}{0}I

I\hfill I

I\hfill I\hfill I

I\hfill I\hfill I\hfill I\hfill I
