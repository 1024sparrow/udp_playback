//#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QUdpSocket>

#include <iostream>

const int PORT = 50100;

const char * const INTERNAL_ERROR_1 = "internal error 1";
const char * const INTERNAL_ERROR_2 = "internal error 2";
const char * const INTERNAL_ERROR_3 = "internal error 3";
const char * const INTERNAL_ERROR_4 = "internal error 4";

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "incorrect argument(s). See sources for details." << std::endl;
		return 1;
	}
	QFile file(argv[1]);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::cerr << "can not open file " << argv[1] << std::endl;
		return 1;
	}
	QMap<int, QByteArray> data;
	{
		int index = 0;
		QByteArray ba;
		QTextStream stream(&file);
		for ( QString line ; stream.readLineInto(&line) ; )
		{
			QString l = line.trimmed();
			if (l.startsWith("#"))
			{
				continue; // commented line
			}
			if (l.isEmpty())
			{
				if (!ba.isEmpty())
				{
					data[index] = ba;
					ba.clear();
				}
				++index;
			}
			else
			{
				l.remove(0,5); // remove address...

				for (QString substr : l.split(" ", QString::SkipEmptyParts))
				{
					bool ok = false;
					int byteCand = substr.toInt(&ok, 16);
					if (!ok)
					{
						std::cerr << INTERNAL_ERROR_1 << std::endl;
						return 1;
					}
					if (byteCand > 255 || byteCand < 0)
					{
						std::cerr << INTERNAL_ERROR_2 << std::endl;
						return 1;
					}
					ba.append(static_cast<char>(byteCand));
				}
			}
		}
		if (!ba.isEmpty())
		{
			data[index] = ba;
			ba.clear();
		}
	}
	file.close();

	QList<QByteArray> order;
	for (int iPacketArg = 2 ; iPacketArg < argc ; ++iPacketArg)
	{
		QString arg = argv[iPacketArg];
		bool ok = false;
		int index = arg.toInt(&ok);
		if (!ok)
		{
			std::cerr << INTERNAL_ERROR_3 << std::endl;
			return 1;
		}
		if (!data.contains(index))
		{
			std::cerr << INTERNAL_ERROR_4 << std::endl;
			return 1;
		}
		order.append(data.value(index));
	}

	//QCoreApplication app(argc, argv);
	QUdpSocket socket;
	for (QByteArray ba : order)
	{
		socket.writeDatagram(ba, QHostAddress::LocalHost, PORT);
	}
	//return app.exec();
	return 0;
}
