
class Essay : IDocumentType {
	Essay() {}

	void section(string heading) {
		pushFont("DejaVuSerif");
		/** \todo write heading **/
		writeString(heading);
		popFont();
	}

}

void section(string heading) {
	pushFont("DejaVuSerif");
	/** \todo write heading **/
	writeString(heading);
	par();
	popFont();
}