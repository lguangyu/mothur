//
//  srainfocommand.cpp
//  Mothur
//
//  Created by Sarah Westcott on 10/29/19.
//  Copyright © 2019 Schloss Lab. All rights reserved.
//

#include "srainfocommand.hpp"
#include "systemcommand.h"

//**********************************************************************************************************************
vector<string> SRAInfoCommand::setParameters(){
    try {
        CommandParameter paccnos("accnos", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(paccnos);
        CommandParameter pPreFetchlocation("prefetch", "String", "", "", "", "", "","",false,false); parameters.push_back(pPreFetchlocation);
        CommandParameter pFasterQlocation("fasterq", "String", "", "", "", "", "","",false,false); parameters.push_back(pFasterQlocation);
        CommandParameter pcompress("gz", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pcompress);
        CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
        CommandParameter pmaxsize("maxsize", "Number", "", "20", "", "", "","",false,false,true); parameters.push_back(pmaxsize);
        CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
        CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["fastq"] = tempOutNames;
        outputTypes["file"] = tempOutNames;
        outputTypes["sra"] = tempOutNames;
        
        vector<string> myArray;
        for (int i = 0; i < parameters.size(); i++) {    myArray.push_back(parameters[i].name);        }
        return myArray;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "setParameters");
        exit(1);
    }
}
//**********************************************************************************************************************
string SRAInfoCommand::getHelpString(){
    try {
        string helpString = "";
        helpString += "The sra.info command reads an accnos file containing sample names. It uses prefetch and fasterq_dump to download and extract the fastq files. The prefetch and fasterq_dump tools developed by NCBI, https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?view=software. Mothur is compatible with version 2.9.6 or greater. \n";
        helpString += "The sra.info command parameters are accnos, fasterq, prefetch, maxsize, gz and processors.\n";
        helpString += "The accnos parameter is used to give the list of samples for download. This file can be generated by clicking on the Accession List button in the Select table on the SRA Run Selector page.\n";
        helpString += "The processors parameter allows you to specify how many processors you would like to use. The default is all available. \n";
        helpString += "The maxsize parameter allows you to limit the size of the files downloaded by prefetch. The default is 20 (20GB). \n";
        helpString += "The gz parameter allows you to compress the fastq files. The default is false. \n";
        helpString += "The fasterq parameter allows you to specify location of the fasterq_dump executable. By default mothur will look in its location and the location of MOTHUR_TOOLS if specified at compile time or set through the set.dir(tools=locationOfExternalTools) command. Ex. sra.info(accnos=SRR_Acc_List.txt.csv, fasterq=/usr/bin/fasterq-dump.2.10.1) or  sra.info(accnos=SRR_Acc_List.txt.csv, fasterq=/usr/local/fasterq_dump). Location and name of exe can be set.\n";
        helpString += "The prefetch parameter allows you to specify location of the prefetch executable. By default mothur will look in its location and the location of MOTHUR_TOOLS if specified at compile time or set through the set.dir(tools=locationOfExternalTools) command. Ex. sra.info(accnos=SRR_Acc_List.txt.csv, prefetch=/usr/bin/prefetch.2.10.1) or  sra.info(accnos=SRR_Acc_List.txt.csv, prefetch=/usr/local/prefetch). Location and name of exe can be set.\n";
        helpString += "The sra.info command should be in the following format: sra.info(accnos=yourAccnosFile)\n";
        helpString += "sra.info(sra=SRR_Acc_List.txt.csv) \n";
        return helpString;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "getHelpString");
        exit(1);
    }
}
//**********************************************************************************************************************
string SRAInfoCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fastq") {  pattern = "[filename],fastq"; }
        else if (type == "file") {  pattern = "[filename],[tag],files"; }
        else if (type == "sra") {  pattern = "[filename],sra"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "getOutputPattern");
        exit(1);
    }
}
//***************************************************************************************************************
SRAInfoCommand::SRAInfoCommand(string option)  {
    try {
        if(option == "help") { help(); abort = true; calledHelp = true; }
        else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
        
        else {
            OptionParser parser(option, setParameters());
            map<string,string> parameters = parser.getParameters();
            
            ValidParameters validParameter;
            accnosfile = validParameter.validFile(parameters, "accnos");
            if (accnosfile == "not open") { accnosfile = ""; abort = true; }
            else if (accnosfile == "not found") { m->mothurOut("[ERROR]: The accnos parameter is required.\n");  abort = true; }
            
            if (outputdir == ""){ outputdir += util.hasPath(accnosfile); }
            
            string temp = validParameter.valid(parameters, "processors");    if (temp == "not found"){    temp = current->getProcessors();    }
            processors = current->setProcessors(temp);
            
            temp = validParameter.valid(parameters, "gz");        if (temp == "not found") { temp = "F"; }
            compressGZ = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "maxsize"); if (temp == "not found"){    temp = "20";    }
            util.mothurConvert(temp, maxSize);
            maxSize *= 1000000;
            
            vector<string> versionOutputs;
            bool foundTool = false;
            string path = current->getProgramPath();
            string programName = "fasterq-dump"; programName += EXECUTABLE_EXT;
            string programVersion = "2.9.6";
#ifdef WINDOWS
            programName = "fastq-dump"; programName += EXECUTABLE_EXT;
#endif
            
            fasterQLocation = validParameter.validFile(parameters, "fasterq");
            if (fasterQLocation == "not found") {
                fasterQLocation = "";
                foundTool = util.findTool(programName, fasterQLocation, path, versionOutputs, current->getLocations());
            }else {
                //test to make sure fasterq exists
                ifstream in;
                fasterQLocation = util.getFullPathName(fasterQLocation);
                bool ableToOpen = util.openInputFile(fasterQLocation, in, "no error"); in.close();
                if(!ableToOpen) {
                    m->mothurOut(fasterQLocation + " file does not exist or cannot be opened, ignoring.\n"); fasterQLocation = "";
                    programName = util.getSimpleName(fasterQLocation); fasterQLocation = "";
                    foundTool = util.findTool(programName, fasterQLocation, path, versionOutputs, current->getLocations());
                }
            }
          
            if (foundTool && !abort) { //check fasterq_dump version
                if (versionOutputs.size() >= 3) {
                    string version = versionOutputs[2];
                                                
                    if (!checkVersion(programVersion, version)) {
                        m->mothurOut("[ERROR]: " + programName + " version found = " + version + ". Mothur requires version " + programVersion + " which is distributed with mothur's executable or available for download here, https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?view=software\n");  abort = true;
                    }else {  m->mothurOut("Using " + programName + " version " + version + ".\n"); }
                }
            }
            
            foundTool = false;
            path = current->getProgramPath();
            programName = "prefetch"; programName += EXECUTABLE_EXT;
            versionOutputs.clear(); programVersion = "2.9.3";
            
            #ifdef WINDOWS
                programName = "prefetch"; programName += EXECUTABLE_EXT;
            #endif
            
            prefetchLocation = validParameter.validFile(parameters, "prefetch");
            if (prefetchLocation == "not found") {
                prefetchLocation = "";
                foundTool = util.findTool(programName, prefetchLocation, path, versionOutputs, current->getLocations());
            }else {
                //test to make sure prefetch exists
                ifstream in;
                prefetchLocation = util.getFullPathName(prefetchLocation);
                bool ableToOpen = util.openInputFile(prefetchLocation, in, "no error"); in.close();
                if(!ableToOpen) {
                    m->mothurOut(prefetchLocation + " file does not exist or cannot be opened, ignoring.\n"); prefetchLocation = "";
                    programName = util.getSimpleName(prefetchLocation); prefetchLocation = "";
                    foundTool = util.findTool(programName, prefetchLocation, path, versionOutputs, current->getLocations());
                }
            }
            
            if (foundTool && !abort) { //check fasterq_dump version
                if (versionOutputs.size() >= 3) {
                    string version = versionOutputs[2];
                    
                    if (!checkVersion(programVersion, version)) {
                        m->mothurOut("[ERROR]: " + programName + " version found = " + version + ". Mothur requires version " + programVersion + " which is distributed with mothur's executable or available for download here, https://trace.ncbi.nlm.nih.gov/Traces/sra/sra.cgi?view=software\n");  abort = true;
                    }else { m->mothurOut("Using " + programName + " version " + version + ".\n"); }
                }
            }
            
            if (!foundTool) { abort = true; }
            
            if (m->getDebug()) { m->mothurOut("[DEBUG]: fasterq-dump location using " + fasterQLocation + "\n"); m->mothurOut("[DEBUG]: prefetch location using " + prefetchLocation + "\n"); }
        }
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "SRAInfoCommand");
        exit(1);
    }
}
//***************************************************************************************************************
int SRAInfoCommand::execute(){
    try{
        
        if (abort) { if (calledHelp) { return 0; }  return 2;    }
        
        set<string> samples = util.readAccnos(accnosfile);

        map<string, string> variables;
        string thisOutputDir = outputdir;
        if (outputdir == "") { thisOutputDir = util.hasPath(accnosfile); }
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(accnosfile));
        variables["[tag]"] = "";
        string fileFileName = getOutputFileName("file",variables);
        ofstream out; util.openOutputFile(fileFileName, out);
        variables["[tag]"] = "single";
        string singleFileFileName = getOutputFileName("file",variables);
        ofstream outSingle; util.openOutputFile(singleFileFileName, outSingle);
        
        int count = 0;
        for (set<string>::iterator it = samples.begin(); it != samples.end(); it++) {
            
            m->mothurOut("\n>>>>>\tProcessing sample " + *it + " (" + toString(count+1) + " of " + toString(samples.size()) + ")\t<<<<<\n"); count++;
            
            string downloadedFile = runPreFetch(*it);
            
            if (downloadedFile != "fail") {
                vector<string> filenames;
                bool hasBoth = runFastqDump(downloadedFile, filenames);
                
                if (hasBoth) {
                    outputNames.push_back(filenames[0]); outputTypes["fastq"].push_back(filenames[0]);
                    outputNames.push_back(filenames[1]); outputTypes["fastq"].push_back(filenames[1]);
                        
                    out << *it << '\t' << util.getSimpleName(filenames[0]) << '\t' << util.getSimpleName(filenames[1]) << endl;
                }else {
                    if (filenames.size() != 0) {
                        outputNames.push_back(filenames[0]); outputTypes["fastq"].push_back(filenames[0]);
                        outSingle << util.getSimpleName(filenames[0]) <<  endl;
                    }
                }
            }
        }
        out.close();
        outSingle.close();
        
        //remove if not filled
        if (util.isBlank(fileFileName)) { util.mothurRemove(fileFileName); }else { outputNames.push_back(fileFileName); outputTypes["file"].push_back(fileFileName); }
        if (util.isBlank(singleFileFileName)) { util.mothurRemove(singleFileFileName); }else { outputNames.push_back(singleFileFileName); outputTypes["file"].push_back(singleFileFileName); }
        
        string currentName = "";
        itTypes = outputTypes.find("file");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFileFile(currentName); }
        }
        
        m->mothurOut("\nOutput File Names: \n");
        for (int i = 0; i < outputNames.size(); i++) {    m->mothurOut(outputNames[i] +"\n");     } m->mothurOutEndLine();
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "execute");
        exit(1);
    }
}
//***************************************************************************************************************
string SRAInfoCommand::runPreFetch(string sampleName){
    try{
        double_t start = time(NULL);
        vector<char*> cPara;
        string prefetchCommand = prefetchLocation;
        prefetchCommand = "\"" + prefetchCommand + "\" " + sampleName + " ";
        
        cPara.push_back(util.mothurConvert(prefetchCommand));
        
        if (maxSize != 20000000) {
            //-X|--max-size <size>             maximum file size to download in KB (exclusive). Default: 20G
            string msize = toString(maxSize);
            cPara.push_back(util.mothurConvert("-X"));
            cPara.push_back(util.mothurConvert(msize));
        }
       
        //-o|--outfile                     output-file
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(sampleName))+".";
        string outputFileName = getOutputFileName("sra",variables);
        
        cPara.push_back(util.mothurConvert("-o"));
        cPara.push_back(util.mothurConvert(outputFileName));
        
        char** preFetchParameters;
        preFetchParameters = new char*[cPara.size()];
        string commandString = "";
        for (int i = 0; i < cPara.size(); i++) {  preFetchParameters[i] = cPara[i];  commandString += toString(cPara[i]) + " "; }
        
#if defined NON_WINDOWS
#else
        commandString = "\"" + commandString + "\"";
#endif
        //free memory
        for(int i = 0; i < cPara.size(); i++)  {  delete cPara[i];  }
        delete[] preFetchParameters;
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: prefetch command = " + commandString + ".\n"); }
        //m->mothurOut("prefetch command = " + commandString + ".\n");
        
        ifstream inTest;
        if (util.openInputFile(outputFileName, inTest, "no error")) { m->mothurOut("\n" + outputFileName + " is found locally, skipping prefetch.\n\n"); return outputFileName; }
            
        //run system command
        runSystemCommand(commandString);
        
        //check for output files
        ifstream in;
        if (!util.openInputFile(outputFileName, in, "no error")) {
            m->mothurOut("\n\n[ERROR]: prefetch was unable to download sample " + sampleName + ", skipping.\n\n"); return "fail";
        }else { outputNames.push_back(outputFileName); outputTypes["sra"].push_back(outputFileName); }
        
        m->mothurOut("It took " + toString(time(NULL)-start)+ " seconds to download sample " + sampleName + ".\n");
        
        return outputFileName;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "runPreFetch");
        exit(1);
    }
}
//***************************************************************************************************************
bool SRAInfoCommand::runFastqDump(string sampleFile, vector<string>& filenames){
    try{
        
        vector<char*> cPara;
        string fasterQCommand = fasterQLocation;
        fasterQCommand = "\"" + fasterQCommand + "\" " + sampleFile + " ";
        cPara.push_back(util.mothurConvert(fasterQCommand));
        
        //-S|--split-files                 write reads into different files
        string splitFiles;
        #if defined NON_WINDOWS
            splitFiles = "-S";
        #else
            splitFiles = "--split-files";
        #endif
        cPara.push_back(util.mothurConvert(splitFiles));
       
		string splitSingleFiles;
#if defined NON_WINDOWS
		//-3|--split-3                     writes single reads in special file
		splitSingleFiles = "-3";
#else
		splitSingleFiles = "--split-3";
#endif
		cPara.push_back(util.mothurConvert(splitSingleFiles));
        
#if defined NON_WINDOWS
        //-e|--threads=processors
        string numProcessors = toString(processors);
        cPara.push_back(util.mothurConvert("-e"));
        cPara.push_back(util.mothurConvert(numProcessors));
#endif       
        #if defined NON_WINDOWS
        #else
            if (compressGZ) { cPara.push_back(util.mothurConvert("-gzip")); }
        #endif
        
        //-o|--outfile                     output-file
	#if defined NON_WINDOWS
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(sampleFile));
        string outputFileName = getOutputFileName("fastq", variables);
        
        cPara.push_back(util.mothurConvert("-o"));
        cPara.push_back(util.mothurConvert(outputFileName));
#else
			if (outputdir != "") {
                string outputFileName = outputdir;
                cPara.push_back(util.mothurConvert("-outdir"));
                cPara.push_back(util.mothurConvert(outputFileName));
			}
#endif
        
        char** fasterQParameters;
        fasterQParameters = new char*[cPara.size()];
        string commandString = "";
        for (int i = 0; i < cPara.size(); i++) {  fasterQParameters[i] = cPara[i];  commandString += toString(cPara[i]) + " "; }
        
#if defined NON_WINDOWS
#else
        commandString = "\"" + commandString + "\"";
#endif
        //free memory
        for(int i = 0; i < cPara.size(); i++)  {  delete cPara[i];  }
        delete[] fasterQParameters;
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: fasterq_dump command = " + commandString + ".\n"); }
        //m->mothurOut("fasterq_dump command = " + commandString + ".\n");
        
        ifstream testfin, testrin;
        string tag = "fastq";
        if (compressGZ) {   tag += ".gz";   }
        string ffastq = outputdir + util.trimStringEnd(util.getRootName(util.getSimpleName(sampleFile)), 1) +"_1." + tag;
        string rfastq = outputdir + util.trimStringEnd(util.getRootName(util.getSimpleName(sampleFile)), 1) +"_2." + tag;
        
        bool found = false;
        //already exist??
        if (util.openInputFile(ffastq, testfin, "no error")) {
            testfin.close();
            if (util.openInputFile(rfastq, testrin, "no error")) {
                m->mothurOut("\n" + ffastq + " and " +  rfastq + " found locally, skipping fasterq_dump.\n\n");
                found = true; testrin.close();
                filenames.push_back(ffastq);
                filenames.push_back(rfastq);
                return found;
            }
        }
        
        if (!found) { runSystemCommand(commandString); }
        
        //fasterq does not have --gzip option, fastq does
        #if defined NON_WINDOWS
            if (compressGZ) { //run system command to compress files
                string tag = "fastq";
                string ffastq = outputdir + util.trimStringEnd(util.getRootName(util.getSimpleName(sampleFile)), 1) +"_1." + tag;
                string rfastq = outputdir + util.trimStringEnd(util.getRootName(util.getSimpleName(sampleFile)), 1) +"_2." + tag;
                
                string inputString = "gzip -f " + ffastq;
                runSystemCommand(inputString);
                util.mothurRemove(ffastq);
                inputString = "gzip -f " + rfastq;
                runSystemCommand(inputString);
                util.mothurRemove(rfastq);
            }
        #else
        #endif
         
        //check for output files. trimstring removes last character
        ifstream fin, rin;
        bool hasBoth = true;
        if (util.openInputFile(ffastq, fin, "no error")) {
            filenames.push_back(ffastq);
            fin.close();
        }else { hasBoth = false; }
        
        if (util.openInputFile(rfastq, rin, "no error")) {
            filenames.push_back(rfastq);
            rin.close();
        }else { hasBoth = false; }
        
        return hasBoth;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "runFastqDump");
        exit(1);
    }
}
/**************************************************************************************************/
//versionNeeded = 2.9.3 versionProvided = 2.10.1
bool SRAInfoCommand::checkVersion(string versionNeeded, string versionProvided){
    try{
        vector<int> versionRequired; vector<int> versionGiven;
        
        vector<string> temps; util.splitAtChar(versionProvided, temps, '.');
        for (int i = 0; i < temps.size(); i++) {
            int thisTemp; util.mothurConvert(temps[i], thisTemp);
            versionGiven.push_back(thisTemp);
        }
        
        temps.clear(); util.splitAtChar(versionNeeded, temps, '.');
        for (int i = 0; i < temps.size(); i++) {
            int thisTemp; util.mothurConvert(temps[i], thisTemp);
            versionRequired.push_back(thisTemp);
        }
        
        //main version tag is too old. 2.9.3, 1.9.1 ie versionNeeded[0] = 2, versionProvided[0] = 1
        if (versionRequired[0] < versionGiven[0]) { return true; }
        else {
            if (versionRequired[0] == versionGiven[0]) { //same major version, check minors
                //minor version tag is too old. 2.9.3, 2.8.1 ie versionNeeded[0] = 9, versionProvided[0] = 8
                if (versionRequired[1] < versionGiven[1]) { return true; }
                else {
                    if (versionRequired[1] == versionGiven[1]) { //same minor version, check next minor
                        //patch version tag is too old. 2.9.3, 2.9.1 ie versionNeeded[0] = 3, versionProvided[0] = 1
                        if (versionRequired[2] <= versionGiven[2]) { return true; }
                    }//else required is greater than given return false
                }
            }//else required is greater than given return false
        }
        
        return false;
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "checkVersion");
        exit(1);
    }
}
/**************************************************************************************************/
void SRAInfoCommand::runSystemCommand(string inputString){
    try{
        m->mothurOut("/******************************************/\n");
        m->mothurOut("\nRunning command: system(" + inputString + ")\n");
        
        system(inputString.c_str());
        
        m->mothurOut("/******************************************/\n");
    }
    catch(exception& e) {
        m->errorOut(e, "SRAInfoCommand", "runSystemCommand");
        exit(1);
    }
}
/**************************************************************************************************/
        
