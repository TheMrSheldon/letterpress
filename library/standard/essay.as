
class Essay : IDocumentType {
	string title;

	Essay(dictionary kwargs) {
		title = string(kwargs["title"]);
		loginfo(string(kwargs["title"]));
	}

	void maketitle() {
		writeString("a");
		writeGlue(0, 100, 0);
		writeString("b");
		writeString(title);
		par();
	}

	void section(string heading) {
		pushFont("cmbx12");
		/** \todo write heading **/
		writeString(heading);
		par();
		popFont();
	}

	void subsection(string heading) {
		pushFont("cmbx12");
		/** \todo write heading **/
		writeString(heading);
		par();
		popFont();
	}

	void paragraph(string heading) {
		pushFont("cmbx12");
		/** \todo write heading **/
		writeString(heading);
		popFont();
	}

}