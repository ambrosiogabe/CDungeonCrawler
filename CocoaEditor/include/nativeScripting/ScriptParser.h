#ifndef COCOA_EDITOR_SCRIPT_PARSER_H
#define COCOA_EDITOR_SCRIPT_PARSER_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/CPath.h"

namespace Cocoa
{
	struct UVariable
	{
		std::string type;
		std::string identifier;
		// void* m_Literal;
	};

	struct UClass
	{
		std::string className;
		CPath fullFilepath;
		std::list<UVariable> variables;
	};

	struct UStruct
	{
		std::string structName;
		const CPath& fullFilepath;
		std::list<UVariable> variables;
	};

	class ScriptParser
	{
	public:
		std::string generateHeaderFile();
		void debugPrint();
		void parse();

		bool canGenerateHeaderFile() const { return mStructs.size() != 0 || mClasses.size() != 0; }
		std::vector<UClass>& getClasses() { return mClasses; }

		static std::string getFilenameAsClassName(std::string filename);

	private:
		int mCurrentToken;
		CPath mFullFilepath;

		std::vector<UClass> mClasses;
		std::vector<UStruct> mStructs;
	};
}

#endif
