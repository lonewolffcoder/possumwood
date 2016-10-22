#include <iostream>
#include <string>
#include <stdexcept>

#include <boost/program_options.hpp>

#include <GL/freeglut.h>

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QAction>

#include <ImathVec.h>

#include "bind.h"
#include "node.h"
#include "graph_widget.h"

namespace po = boost::program_options;

using std::cout;
using std::endl;
using std::flush;

namespace {

QAction* makeAction(QString title, std::function<void()> fn) {
	QAction* result = new QAction(title, NULL);
	qt_bind(result, SIGNAL(triggered(bool)), fn);
	return result;
}

QString makeUniqueNodeName() {
	static unsigned s_counter = 0;
	++s_counter;

	return "node_" + QString::number(s_counter);
}

}

int main(int argc, char* argv[]) {
	// // Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
	("help", "produce help message")
	;

	// process the options
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if(vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	///////////////////////////////

	QApplication app(argc, argv);

	// make a window only with a viewport
	QMainWindow win;

	win.showMaximized();

	///

	GraphWidget* graph = new GraphWidget(&win);
	win.setCentralWidget(graph);

	Node& n1 = graph->addNode("first", {{"aaaaa", Port::kInput, Qt::blue}, {"b", Port::kOutput, Qt::red}}, QPointF(-50, 20));
	Node& n2 = graph->addNode("second", {{"xxxxxxxxxxxxxxxx", Port::kInputOutput, Qt::red}}, QPointF(50, 20));

	graph->connect(n1.port(1), n2.port(0));

	///

	graph->setContextMenuCallback([&](QPoint p) {
		QMenu menu(graph);

		menu.addAction(makeAction("Add single input node", [&]() {
			graph->addNode(makeUniqueNodeName(), {{"input", Port::kInput, Qt::blue}});
		}));

		menu.addAction(makeAction("Add single output node", [&]() {
			graph->addNode(makeUniqueNodeName(), {{"output", Port::kOutput, Qt::blue}});
		}));

		menu.addAction(makeAction("Add more complex node", [&]() {
			graph->addNode(makeUniqueNodeName(), {
				{"red_input", Port::kInput, Qt::red},
				{"red_pass_through", Port::kInputOutput, Qt::red},
				{"blue_pass_through", Port::kInputOutput, Qt::blue},
				{"blue_output", Port::kOutput, Qt::blue},
				{"red_output", Port::kOutput, Qt::red}
			});
		}));

		menu.exec(p);
	});

	///

	app.exec();

	return 0;
}
