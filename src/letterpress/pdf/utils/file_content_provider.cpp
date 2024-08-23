#include <letterpress/pdf/utils/file_content_provider.hpp>

#include <qpdf/Pl_Flate.hh>

#include <fstream>
#include <memory>
#include <vector>

using namespace lp::pdf::utils;

FileContentProvider::FileContentProvider(std::string path, Filter filter)
		: StreamDataProvider(false), path(path), filter(filter) {}

void FileContentProvider::provideStreamData(QPDFObjGen const&, Pipeline* pipeline) {
	std::vector<std::shared_ptr<Pipeline>> to_delete;
	Pipeline* p = pipeline;
	std::shared_ptr<Pipeline> p_new;

	if (filter == Filter::FlateDecode) {
		p_new = std::make_shared<Pl_Flate>("font file", pipeline, Pl_Flate::action_e::a_deflate);
		to_delete.push_back(p_new);
		p = p_new.get();
	}

	std::ifstream file(path, std::ios::binary);
	char buffer[4048];
	while (!file.eof()) {
		file.read(buffer, sizeof(buffer));
		p->write(buffer, file.gcount());
	}
	file.close();
	p->finish();
}