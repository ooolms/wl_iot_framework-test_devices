/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "StdQFile.h"
#include <unistd.h>
#include <QDebug>

StdQFile StdQFile::instVar=StdQFile();

StdQFile::StdQFile()
{
	mStdout=new QFile;
	mStdout->open(STDOUT_FILENO,QIODevice::WriteOnly);
	mStdin=new QFile;
	mStdin->open(STDIN_FILENO,QIODevice::ReadOnly);
	mStderr=new QFile;
	mStderr->open(STDERR_FILENO,QIODevice::WriteOnly);
}

StdQFile::~StdQFile()
{
	delete mStdout;
	delete mStdin;
	delete mStderr;
}

QFile* StdQFile::stdout()
{
	return instVar.mStdout;
}

QFile* StdQFile::stdin()
{
	return instVar.mStdin;
}

QFile* StdQFile::stderr()
{
	return instVar.mStderr;
}

QDebug StdQFile::stdoutDebug()
{
	return QDebug(instVar.mStdout).noquote().nospace();
}

QDebug StdQFile::stderrDebug()
{
	return QDebug(instVar.mStderr).noquote().nospace();
}
