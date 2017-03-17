/*
 * CX_FileInformation.h
 *
 *  Created on: Jul 8, 2013
 *      Author: joost
 */

#ifndef CX_FILEINFORMATION_H_
#define CX_FILEINFORMATION_H_

class FileInformation {
public:
	FileInformation(bool _newFile = false, std::string _dataVersion="0.0", std::string _age = "0", std::string _phenotype = "grey", std::string _branch="0", std::string _id="0"){
		newFile=_newFile;
		dataVersion = _dataVersion;
		age = _age;
		phenotype = _phenotype;
		branch = _branch;
		id = _id;
	}
//	virtual ~FileInformation();

	void init(std::iostream &stream){
		std::string cppnxDataVersion;
		stream >> cppnxDataVersion;
		stream >> dataVersion;
		stream >> age;
		stream >> phenotype;
		stream >> branch;
		stream >> id;

		if(cppnxDataVersion == "0.0"){
			newFile=true;
		}
	}

	void addParent(std::string branch = "unknown", std::string id = "unknown"){
		parent_branches.push_back(branch);
		parent_ids.push_back(id);
	}

	void setGenome(std::string _age = "unknown", std::string _phenotype = "grey"){
		age = _age;
		phenotype = _phenotype;
	}

	void setIdentifier(std::string _branch = "unknown", std::string _id = "unknown"){
		branch = _branch;
		id = _id;
	}

    bool newFile;
    QString fileName;
    std::string age;
    std::string phenotype;
    std::string branch;
    std::string id;
    std::string dataVersion;
    std::vector<std::string> parent_branches;
    std::vector<std::string> parent_ids;
};

#endif /* CX_FILEINFORMATION_H_ */
