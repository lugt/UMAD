#include "taskfun.h"


//task inteface for build and search MVPTree
void
bastask(basarg * args)
{
	char buffer[MAXLINE];

	write(args->socketfd, args->argstr->c_str(), strlen(args->argstr->c_str()));

	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);

	if (strcmp(buffer, args->rawdatafilename) == 0)
	{
		fstream dataf(args->rawdatafilename);

		for (int i=args->bstart; i<args->bend; i++)
		{
			bzero(buffer, MAXLINE);
			dataf.read(buffer, MAXLINE);

			write(args->socketfd, buffer, MAXLINE);
		}
	}

	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);
	fstream dataf("num.txt");	
	if (strcmp(buffer, args->querydatafilename) == 0)
	{
		for (int i=0; i<args->send; i++)
		{
			bzero(buffer, MAXLINE);
			dataf.read(buffer, MAXLINE);

			write(args->socketfd, buffer, MAXLINE);
		}
	}
	
	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);	
	
	stringstream ss(buffer);
	ss >> args->btime;
	ss >> args->indexfilename;

	ss >> args->stotaltime;
	ss >> args->sresponsetime;
	ss >> args->sresultnum;
};

//task inter face for building a MVPTree
void
btask(struct barg *args)
{
    	char buffer[MAXLINE];

	write(args->socketfd, args->argstr->c_str(), strlen(args->argstr->c_str()));

	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);

	if (strcmp(buffer, args->rawdatafilename) == 0)
	{
		fstream dataf(args->rawdatafilename);

		for (int i=args->bstart; i<args->bend; i++)
		{
			bzero(buffer, MAXLINE);
			dataf.read(buffer, MAXLINE);

			write(args->socketfd, buffer, MAXLINE);
		}
	}

	stringstream ss(buffer);
	ss >> args->btime;
	ss >> args->indexfilename;
};

//task interface for searching in a previously built MVPTree
void
stask(struct sarg *args)
{
	char buffer[MAXLINE];

	write(args->socketfd, args->argstr->c_str(), strlen(args->argstr->c_str()));
	cout << __LINE__ << endl;
	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);
	fstream dataf("num.txt");	
	if (strcmp(buffer, args->querydatafilename) == 0)
	{
		for (int i=0; i<args->send; i++)
		{
			bzero(buffer, MAXLINE);
			dataf.read(buffer, MAXLINE);

			write(args->socketfd, buffer, MAXLINE);
		}
	}
	cout << __LINE__ << endl;
	bzero(buffer, MAXLINE);
	read(args->socketfd, buffer, MAXLINE);	
	
	stringstream ss(buffer);

	ss >> args->stotaltime;
	ss >> args->sresponsetime;
	ss >> args->sresultnum;

};

unordered_map<long, std::pair<char*,CIndexNode*> > indexCache;

//for results
stringstream strtrans;


void 
bastask(string &dataFileName, int &numPivot, int &setE, int &setC, int &singlePivotFanout, int &fftscale, string &pivotSelectionMethod, string &partitionMethod, int &maxLeafSize, string &dataType, int &initialSize, int &finalSize, int &stepSize, bool &bucket, int &fragmentLength, int &dim, double &maxR, string &indexType, int &fftOpt, string &queryFileName, int &firstQuery, int &lastQuery, double &maxRadius, double &minRadius, double &step, bool &verify, string &resultsFileName, int &buildMode, string &indexName, int &searchMode, double &MF_maxRadius, double &MF_middleProportion, bool &putInfo, int &cacheHeight, int &tsThreadNum, bool &cacheAll, int &bThreadNum, double &trisectionRadius, string &selectOptimalPivots, string &partitionByOnePivot,int &numberOfChildrenPathSearchingThreads, string &resultcontainer)
{
	char *ndatatype = new char[dataType.size()];
	strcpy(ndatatype, dataType.c_str());

	char *nindextype = new char[indexType.size()];
	strcpy(nindextype, indexType.c_str());

	char *nindexname = new char[indexName.size()];
	strcpy(nindexname, indexName.c_str());

	char *npivotSelectionMethod = new char[pivotSelectionMethod.size()];
	strcpy(npivotSelectionMethod, pivotSelectionMethod.c_str());
	char *npartitionMethod = new char[partitionMethod.size()];
	strcpy(npartitionMethod, partitionMethod.c_str());

	char *nselectOptimalPivots = new char[selectOptimalPivots.size()];
	strcpy(nselectOptimalPivots, selectOptimalPivots.c_str());

	char *npartitionByOnePivot = new char[partitionByOnePivot.size()];
	strcpy(npartitionByOnePivot, partitionByOnePivot.c_str());

	//cout << "datatype:" << ndatatype << endl;
	//cout << "nindextype:" << nindextype << endl;
	//cout << "nindexname:" << nindexname << endl;
	//cout << "resultname:" << resultsFileName << endl;
	//cout << "npivotSelectionMethod:" << pivotSelectionMethod << endl;
	//cout << "npartitionMethod:" << partitionMethod << endl;
	//cout << "nselectOptimalPivots:" << selectOptimalPivots << endl;
	//cout << "npartitionByOnePivot:" << partitionByOnePivot << endl;

	vector< shared_ptr<CMetricData> > *rawData=0;

	CMetricDistance *metric=0;

	CPivotSelectionMethod *psm=0;

	CPartitionMethod *pm=0;

	CIndex *index =0;

	int indexNum = (finalSize - initialSize) / stepSize + 1;

	double *loadDataTimes=new double[indexNum];

	double *buildTimes=new double[indexNum];

	char *newDataFileName="../data/";

	joinCharArray(newDataFileName, dataFileName.c_str());

	char *newQueryFile="../data/";

	joinCharArray(newQueryFile, queryFileName.c_str());

	char *newIndexName="../database/";

	joinCharArray(newIndexName, indexName.c_str());

	char *resultFile="../result/";

	joinCharArray(resultFile, resultsFileName.c_str());

	ofstream output(resultFile, ofstream::app);

	vector<shared_ptr<CMetricData> > *queryList =  NULL;


	for (int size = initialSize, i = 0; (size <= finalSize) & (i < indexNum); size += stepSize, i++)
	{

#ifdef __GNUC__
		clock_t beginTime1 = times(NULL);
#else
#ifdef _WIN32
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalStartTime,&userStartTime);
#endif
#endif
		loadDataByType(ndatatype, rawData, newDataFileName, size, dim, fragmentLength);
		loadDataByType(ndatatype, queryList, newQueryFile, lastQuery, dim, fragmentLength);
#ifdef __GNUC__
			clock_t endTime1 = times(NULL);
			loadDataTimes[i]= (endTime1-beginTime1)/100.0;
#else
#ifdef _WIN32
			GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalEndTime,&userEndTime);
			loadDataTimes[i]=((userEndTime.dwLowDateTime + kernalEndTime.dwLowDateTime) - (userStartTime.dwLowDateTime + kernalStartTime.dwLowDateTime)) / 10000000.00;
#endif
#endif

#ifdef __GNUC__
		CMemMonitor *memoryMonitor = new CMemMonitor();
		memoryMonitor->getMemorySize();
		output<<"=================Loading Data Used Time And Memory================="<<endl;
		output<<"loadDataTime: "<<loadDataTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize: "<<memoryMonitor->resMemorySize <<" MB / PeakWorkingSetSize: "<<memoryMonitor->peekResMemorySize<<" MB "<<endl; 
		/*output<<"swapOutSize: "<<memoryMonitor->swapMemorySize<<" MB"<<endl;*/
		output<<"MemoryUsage: "<<memoryMonitor->virtMemorySize <<" MB/ PeakMemoryUsage: "<< memoryMonitor->peekVirtMemorySize <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
#else
#ifdef _WIN32
		GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(pmc));
		output<<"=================Loading Data Used Time And Memory================="<<endl;
		output<<"loadDataTime: "<<loadDataTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize: "<<pmc.WorkingSetSize/1048576.0 <<" MB / PeakWorkingSetSize: "<<pmc.PeakWorkingSetSize/1048576.0<<" MB "<<endl; 
		output<<"PagefileUsage: "<<pmc.PagefileUsage/1048576.0<<" MB/ PeakPagefileUsage: "<< pmc.PeakPagefileUsage/1048576.0<<" MB "<<endl;
		output<<"MemoryUsage: "<<pmc.PagefileUsage/1048576.0 + pmc.WorkingSetSize/1048576.0<<" MB/ PeakMemoryUsage: "<< pmc.PeakPagefileUsage/1048576.0 + pmc.PeakWorkingSetSize/1048576.0 <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
#endif
#endif

		getMetricByType(metric,ndatatype);
		CountedMetric *cmetric=new CountedMetric(metric);

		getPivotSelectionMethod(npivotSelectionMethod, psm, fftscale, setE, setC) ;
		getPartitionMethod(npartitionMethod, pm);
		getIndexType(metric,cmetric,rawData,psm,pm,index,ndatatype,nindextype,newIndexName,numPivot,singlePivotFanout,maxLeafSize,MF_maxRadius,MF_middleProportion,buildMode,bThreadNum,trisectionRadius,nselectOptimalPivots,npartitionByOnePivot);

		int distanceFunctionCounterBeforSearch=0,distanceFunctionCounterAfterSearch=0;

		distanceFunctionCounterBeforSearch=dynamic_cast<CountedMetric*>(index->getMetric())->getCounter();


#ifdef _WIN32
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalStartTime,&userStartTime);
		index->bulkLoad(*rawData,buildMode,bThreadNum);
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalEndTime,&userEndTime);
		buildTimes[i] = ((userEndTime.dwLowDateTime + kernalEndTime.dwLowDateTime) - (userStartTime.dwLowDateTime + kernalStartTime.dwLowDateTime))/10000000.00;
		distanceFunctionCounterAfterSearch=dynamic_cast<CountedMetric*>(index->getMetric())->getCounter();

		GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(pmc));
		output<<"=================Building index Used Time And Memory================="<<endl;
		output<<"datasize: "<<size<<endl;
		output<<"distCalculateTimes: "<<distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch<<endl;
		output<<"buildTime: "<<buildTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize: "<<pmc.WorkingSetSize/1048576.0 <<" MB / PeakWorkingSetSize: "<<pmc.PeakWorkingSetSize/1048576.0<<" MB "<<endl; 
		output<<"PagefileUsage: "<<pmc.PagefileUsage/1048576.0<<" MB/ PeakPagefileUsage: "<< pmc.PeakPagefileUsage/1048576.0<<" MB "<<endl;
		output<<"MemoryUsage: "<<pmc.PagefileUsage/1048576.0 + pmc.WorkingSetSize/1048576.0<<" MB/ PeakMemoryUsage: "<< pmc.PeakPagefileUsage/1048576.0 + pmc.PeakWorkingSetSize/1048576.0 <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
		delete memoryMonitor;

		strtrans.str("");
		strtrans << newIndexName << " " <<buildTimes[i] << " ";
		

#else
#ifdef __GNUC__
		clock_t beginTime = times(NULL);
		index->bulkLoad(*rawData,buildMode,bThreadNum);
		clock_t endTime = times(NULL);
		buildTimes[i]= (endTime-beginTime)/100.0;
		distanceFunctionCounterAfterSearch=dynamic_cast<CountedMetric*>(index->getMetric())->getCounter();

		strtrans << indexName << " " <<buildTimes[i] << " ";
		

		output<<"=================Building index Used Time And Memory================="<<endl;
		output<<"datasize: "<<size<<endl;
		output<<"distCalculateTimes: "<<distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch<<endl;
		output<<"buildTime: "<<buildTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize: "<<memoryMonitor->resMemorySize <<" MB / PeakWorkingSetSize: "<<memoryMonitor->peekResMemorySize<<" MB "<<endl; 
		//output<<"swapOutSize: "<<memoryMonitor->swapMemorySize<<" MB";
		output<<"MemoryUsage: "<<memoryMonitor->virtMemorySize <<" MB/ PeakMemoryUsage: "<< memoryMonitor->peekVirtMemorySize <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
		delete memoryMonitor;
#endif
#endif
		


		if(putInfo==1)
			getMVPIndexStructureInfo(newIndexName,ndatatype,resultFile);

		_search(queryList,cmetric,resultFile,newIndexName,newDataFileName,dim,fragmentLength,verify,ndatatype,maxRadius,minRadius,step,searchMode,firstQuery,lastQuery,size,cacheHeight,tsThreadNum,numberOfChildrenPathSearchingThreads, resultcontainer);

		delete cmetric;
	}

	delete [] ndatatype;
	delete [] nindextype;
	delete [] nindexname;
	delete [] npivotSelectionMethod;
	delete [] npartitionMethod;
	delete [] nselectOptimalPivots;
	delete [] npartitionByOnePivot;
	delete [] buildTimes;
	delete [] loadDataTimes;
	delete [] buildTimes;

	free(newDataFileName);
	free(newQueryFile);
	free(newIndexName);
	free(resultFile);

}

void 
stask(int &dataBasehSize, string &queryFileName, string &dataType, int &firstQuery, int &lastQuery, int &dim, int &fragmentLength, double &maxRadius, double &minRadius, double &step, bool &verify, string &resultsFileName, int &buildMode, string &indexName, int &searchMode, int &cacheHeight, int &tsThreadNum, bool &cacheAll, int &numberOfChildrenPathSearchingThreads, string &resultcontainer)
{
	char *ndatatype = new char[dataType.size()];
	strcpy(ndatatype, dataType.c_str());

	char *fileName="../database/";
	joinCharArray(fileName,indexName.c_str());

	char *resultFile="../result/";
	joinCharArray(resultFile,resultsFileName.c_str());

	char *newQueryFile="../data/";
	joinCharArray(newQueryFile,queryFileName.c_str());

	CMetricDistance *metric=0;

	vector<shared_ptr<CMetricData> > linerDataList;


	vector<shared_ptr<CMetricData> > *queryList = NULL;
	loadDataByType(ndatatype,queryList, newQueryFile,lastQuery,dim,fragmentLength) ;
	getMetricByType(metric,ndatatype);
	CountedMetric *cmetric = new CountedMetric(metric);
	
	strtrans.str("");
	_search(queryList,cmetric,resultFile,fileName,newQueryFile,dim,fragmentLength,verify,ndatatype,maxRadius,minRadius,step,searchMode,firstQuery,lastQuery,dataBasehSize,cacheHeight,tsThreadNum,numberOfChildrenPathSearchingThreads, resultcontainer);
	delete [] ndatatype;

	delete cmetric;

	free(fileName);
	free(newQueryFile);
	free(resultFile);
}

void 
btask(string &dataFileName, int &numPivot, int &setE, int &setC, int &singlePivotFanout, int &fftscale, string &pivotSelectionMethod, string &partitionMethod,int &maxLeafSize, string &dataType, int &initialSize, int &finalSize, int &stepSize, bool &bucket, int &fragmentLength, int &dim, double &maxR, string &indexType, int &fftOpt, string &buildingInformationFile, int &buildMode, string &indexName, double &MF_maxRadius, double &MF_middleProportion, bool &putInfo, int &bThreadNum, double &trisectionRadius,string &selectOptimalPivots,string &partitionByOnePivot, string &resultcontainer)
{
	char *ndatatype = new char[dataType.size()];
	strcpy(ndatatype, dataType.c_str());
	
	char *nindextype = new char[indexType.size()];
	strcpy(nindextype, indexType.c_str());
	
	char *nindexname = new char[indexName.size()];
	strcpy(nindexname, indexName.c_str());

	char *npivotSelectionMethod = new char[pivotSelectionMethod.size()];
	strcpy(npivotSelectionMethod, pivotSelectionMethod.c_str());
	char *npartitionMethod = new char[partitionMethod.size()];
	strcpy(npartitionMethod, partitionMethod.c_str());

	char *nselectOptimalPivots = new char[selectOptimalPivots.size()];
	strcpy(nselectOptimalPivots, selectOptimalPivots.c_str());

	char *npartitionByOnePivot = new char[partitionByOnePivot.size()];
	strcpy(npartitionByOnePivot, partitionByOnePivot.c_str());

	vector<shared_ptr<CMetricData> > *rawData=0;

	CMetricDistance *metric=0;

	CPivotSelectionMethod *psm=0;

	CPartitionMethod *pm=0;

	CIndex *index =0;

	int indexNum = (finalSize - initialSize) / stepSize + 1;

	double *buildTimes=new double[indexNum];

	double *loadDataTimes=new double[indexNum];

	int *distanceCalculatedTimes=new int[indexNum];


	char *newDataFileName="../data/";

	joinCharArray(newDataFileName,dataFileName.c_str());



	char *newIndexName="../database/";

	joinCharArray(newIndexName,indexName.c_str());

	//cout<<__FILE__<<__LINE__<<" build index file name:"<<newIndexName<<endl;



	char *resultFileName="../result/";

	joinCharArray(resultFileName,buildingInformationFile.c_str());

	ofstream output(resultFileName,ofstream::app);

	int distanceFunctionCounterBeforSearch=0,distanceFunctionCounterAfterSearch=0;

	getMetricByType(metric,ndatatype);
	CountedMetric *cmetric = new CountedMetric(metric);

	for (int size = initialSize, i = 0; (size <= finalSize) & (i < indexNum); size += stepSize, i++)
	{


#ifdef __GNUC__
		clock_t beginTime = times(NULL);
#else
#ifdef _WIN32
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalStartTime,&userStartTime);
#endif
#endif
		loadDataByType(ndatatype, rawData, newDataFileName,size,dim,fragmentLength);

#ifdef __GNUC__
			clock_t endTime = times(NULL);
			loadDataTimes[i]= (endTime-beginTime)/100.0;
#else
#ifdef _WIN32
			GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalEndTime,&userEndTime);
			loadDataTimes[i]=((userEndTime.dwLowDateTime + kernalEndTime.dwLowDateTime) - (userStartTime.dwLowDateTime + kernalStartTime.dwLowDateTime)) / 10000000.00;
#endif
#endif



#ifdef __GNUC__
		CMemMonitor *memoryMonitor = new CMemMonitor();

		memoryMonitor->getMemorySize();

		output<<"=================Loading Data Used Time And Memory================="<<endl;

		output<<"loadDataTime: "<<loadDataTimes[i]<<" s"<<endl;

		output<<"WorkingSetSize: "<<memoryMonitor->resMemorySize <<" MB / PeakWorkingSetSize: "<<memoryMonitor->peekResMemorySize<<" MB "<<endl; 

		/*output<<"swapOutSize: "<<memoryMonitor->swapMemorySize<<" MB"<<endl;*/

		output<<"MemoryUsage: "<<memoryMonitor->virtMemorySize <<" MB/ PeakMemoryUsage: "<< memoryMonitor->peekVirtMemorySize <<" MB "<<endl;

		output<<"==================================================================="<<endl<<endl;
#else
#ifdef _WIN32
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		output<<"=================Loading Data Used Time And Memory================="<<endl;
		output<<"loadDataTime  :"<<loadDataTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize:"<<pmc.WorkingSetSize/1048576.0 <<" MB / PeakWorkingSetSize: "<<pmc.PeakWorkingSetSize/1048576.0<<" MB "<<endl; 
		output<<"PagefileUsage :"<<pmc.PagefileUsage/1048576.0<<" MB/ PeakPagefileUsage: "<< pmc.PeakPagefileUsage/1048576.0<<" MB "<<endl;
		output<<"MemoryUsage   :"<<pmc.PagefileUsage/1048576.0 + pmc.WorkingSetSize/1048576.0<<" MB/ PeakMemoryUsage: "<< pmc.PeakPagefileUsage/1048576.0 + pmc.PeakWorkingSetSize/1048576.0 <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
#endif
#endif

		getPivotSelectionMethod(npivotSelectionMethod,psm,fftscale,setE,setC) ;
		getPartitionMethod(npartitionMethod,pm);

		getIndexType(metric,cmetric,rawData,psm,pm,index,ndatatype,nindextype,newIndexName,numPivot,singlePivotFanout,maxLeafSize,MF_maxRadius,MF_middleProportion,buildMode,bThreadNum,trisectionRadius,nselectOptimalPivots,npartitionByOnePivot);

		distanceFunctionCounterBeforSearch=dynamic_cast<CountedMetric*>(index->getMetric())->getCounter();

#ifdef __GNUC__
		clock_t beginTime1 = times(NULL);
		index->bulkLoad(*rawData,buildMode,bThreadNum);
		clock_t endTime1 = times(NULL);
		buildTimes[i]= (endTime1-beginTime1)/100.0;
#else
#ifdef _WIN32
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalStartTime,&userStartTime);
		index->bulkLoad(*rawData,buildMode,bThreadNum);;
		GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalEndTime,&userEndTime);
		buildTimes[i]=((userEndTime.dwLowDateTime + kernalEndTime.dwLowDateTime) - (userStartTime.dwLowDateTime + kernalStartTime.dwLowDateTime)) / 10000000.00;
#endif
#endif

		distanceFunctionCounterAfterSearch=dynamic_cast<CountedMetric*>(index->getMetric())->getCounter();

		distanceCalculatedTimes[i]=distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch;

		if(putInfo==1)
			getMVPIndexStructureInfo(newIndexName,ndatatype,resultFileName);

		strtrans.str("");
		strtrans << indexName << " " << buildTimes[i] << " ";

#ifdef __GNUC__
		memoryMonitor->getMemorySize();
		output<<"=================Building index Used Time And Memory================="<<endl;
		output<<"datasize: "<<size<<endl;
		output<<"distCalculateTimes: "<<distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch<<endl;
		output<<"buildTime: "<<buildTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize: "<<memoryMonitor->resMemorySize <<" MB / PeakWorkingSetSize: "<<memoryMonitor->peekResMemorySize<<" MB "<<endl; 
		/*output<<"swapOutSize: "<<memoryMonitor->swapMemorySize<<" MB"<<endl;*/
		output<<"MemoryUsage: "<<memoryMonitor->virtMemorySize <<" MB/ PeakMemoryUsage: "<< memoryMonitor->peekVirtMemorySize <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
		delete memoryMonitor;
#else
#ifdef _WIN32
		GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(pmc));

		output<<"=================Building index Used Time And Memory================="<<endl;
		output<<"datasize          :"<<size<<endl;
		output<<"distCalculateTimes:"<<distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch<<endl;
		output<<"buildTime         :"<<buildTimes[i]<<" s"<<endl;
		output<<"WorkingSetSize    :"<<pmc.WorkingSetSize/1048576.0 <<" MB / PeakWorkingSetSize: "<<pmc.PeakWorkingSetSize/1048576.0<<" MB "<<endl; 
		output<<"PagefileUsage     :"<<pmc.PagefileUsage/1048576.0<<" MB/ PeakPagefileUsage: "<< pmc.PeakPagefileUsage/1048576.0<<" MB "<<endl;
		output<<"MemoryUsage       :"<<pmc.PagefileUsage/1048576.0 + pmc.WorkingSetSize/1048576.0<<" MB/ PeakMemoryUsage: "<< pmc.PeakPagefileUsage/1048576.0 + pmc.PeakWorkingSetSize/1048576.0 <<" MB "<<endl;
		output<<"==================================================================="<<endl<<endl;
		delete memoryMonitor;
#endif
#endif
       
        
	}
	
	resultcontainer.append(strtrans.str().c_str());
	strtrans.str("");

	delete [] ndatatype;
	delete [] nindextype;
	delete [] nindexname;
	delete [] npivotSelectionMethod;
	delete [] npartitionMethod;
	delete [] nselectOptimalPivots;
	delete [] npartitionByOnePivot;
	delete [] buildTimes;
	delete [] loadDataTimes;
	delete [] distanceCalculatedTimes;

	delete cmetric;
	
	free(newDataFileName);
	free(newIndexName);
	free(resultFileName);
}

//serch 
void 
_search(vector<shared_ptr<CMetricData> > *queryList,CountedMetric *cmetric,char *resultFileName,char *indexFileName,char *rawDataFile,int dim,int fragmentLength,bool verify,char *dataType,double maxRadius,double minRadius,double step,int searchMode,int firstQuery,int lastQuery,int dataBaseSize,int cacheHeight,int maximumThreadsNumForSearching,int totalSearchingThreadForChildPathSearchingTaskInThreadpool, string &resultcontainer)
{
	int distanceFunctionCounterBeforSearch=0,distanceFunctionCounterAfterSearch=0;

	int numRun=ceil((maxRadius-minRadius)/step)+1;

	double *mvpqueryTime=new double[numRun];

	double *maxQueryTime=new double[numRun];

	double *minQueryTime=new double[numRun];

	double *mvpdistanceCalculatedTimes=new double[numRun];

	double *maxDistanceCalculatedTimes=new double[numRun];

	double *minDistanceCalculatedTimes=new double[numRun];

	double *avgpeakWorkingSetSize=new double[numRun];

	double *peakWorkingSetSize=new double[numRun];


	double *peekmemoryUseSize=new double[numRun];

	double *avgpagefileusage=new double[numRun];

	double *peakpagefileusage=new double[numRun];

	double *avgpeekmemoryUseSize=new double[numRun];

	double *averageMVPSearchCount=new double[numRun];

	ofstream output(resultFileName,ofstream::app);

	vector<shared_ptr<CMetricData> > *rawData=0;

	if(verify) 
		loadDataByType(dataType, rawData, rawDataFile, dataBaseSize, dim, fragmentLength) ;

	output<<"datasize   "<<"searchradius   "<<"distcalculation   "<<"searchtime   "<<"searchresults   "<<"avgpeakworkingsetsize   "<<"peakworkingsetsize   "<<"avgpeekmemoryUseSize   "<<"peekmemoryUseSize   "<<endl;
	if(searchMode<=2)
	{
		strtrans << numRun << " ";
		//cout << __LINE__ <<" ss.size:" << strlen(strtrans.str().c_str()) << " " << strtrans.str() << endl;

		for(int i=0;i<numRun;i++)
		{
			strtrans << (minRadius + step * i) << " ";
			//cout << __LINE__ <<" ss.size:" << strlen(strtrans.str().c_str()) << " " << strtrans.str() << endl;

			mvpqueryTime[i]=0;

			maxQueryTime[i]=DBL_MIN;

			minQueryTime[i]=DBL_MAX;

			mvpdistanceCalculatedTimes[i]=0;


			maxDistanceCalculatedTimes[i]=DBL_MIN;

			minDistanceCalculatedTimes[i]=DBL_MAX;

			averageMVPSearchCount[i] = 0.0;



			peakWorkingSetSize[i] = 0.0;

			avgpeakWorkingSetSize[i] = 0.0;



			peekmemoryUseSize[i] = 0.0;

			avgpeekmemoryUseSize[i] = 0.0;



			peakpagefileusage[i] = 0.0;

			avgpagefileusage[i] = 0.0;





			double searchRadius=minRadius+step*i;

			int count=1;

			double t = 0.0;

			int startQuery=(firstQuery<0) ? 0:firstQuery;

			int endQuery=(lastQuery>queryList->size()) ? queryList->size():lastQuery;



			vector<shared_ptr<CMetricData> > newQueryList;

			vector<shared_ptr<CMetricData> >::iterator ite=queryList->begin();

			newQueryList.insert(newQueryList.begin(),ite+startQuery,ite+endQuery);

			ite=newQueryList.begin();

#ifdef __GNUC__
			clock_t beginTTime = times(NULL);
#endif
			for(;ite!=newQueryList.end();ite++)
			{

				CMemMonitor::reset();

				CRangeQuery q(minRadius+step*i,*ite);

				list<shared_ptr<CMetricData> >* mvptreeSearchResults = new list<shared_ptr<CMetricData> >();

				long rootadd = 0;
				ifstream in(indexFileName, ios::binary);
				in.seekg(-(int)sizeof(long), ios::end);
				in.read((char*)&rootadd, sizeof(long));
				in.close();

				cout << "rootad:" << rootadd << endl;
			

				distanceFunctionCounterBeforSearch=dynamic_cast<CountedMetric*>(cmetric)->getCounter();

#ifdef _WIN32
				GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalStartTime,&userStartTime);
				
				if (searchMode == 0)
				{
					shared_ptr<CMVPIndex>  mvpIndex(new CMVPIndex());

					mvpIndex->setFileName(indexFileName);

					mvpIndex->setDataType(dataType);

					mvpIndex->setMetric(cmetric);

					cout << " start search "
					mvptreeSearchResults = mvpIndex->search(&q);
					cout << " end  search " << endl;
					CMemMonitor::updateMem();

					averageMVPSearchCount[i] += mvptreeSearchResults->size();

				}
				else if (searchMode == 1)
				{

					shared_ptr< CSearchTask > searchTask (new CSearchTask(indexFileName,dataType,*cmetric));

					list<shared_ptr<CMetricData> >* searchResults = searchTask->search(q);

					CMemMonitor::updateMem();

					averageMVPSearchCount[i] += searchResults->size();

					list<shared_ptr<CMetricData> >::iterator mvp_iter = searchResults->begin();
					for(;mvp_iter!=searchResults->end();mvp_iter++)
					{
						mvptreeSearchResults->push_back(*mvp_iter);
					}

				}
				else 
				{

					shared_ptr<CExcludedMiddleForest>  ForestIndex(new CExcludedMiddleForest());

					ForestIndex->setFileName(indexFileName);

					ForestIndex->setDataType(dataType);

					ForestIndex->setMetric(cmetric);

					list<shared_ptr<CMetricData> >* searchResults = ForestIndex->search(&q);

					CMemMonitor::updateMem();

					averageMVPSearchCount[i] += searchResults->size();

					list<shared_ptr<CMetricData> >::iterator mvp_iter = searchResults->begin();
					for(;mvp_iter!=searchResults->end();mvp_iter++)
					{
						mvptreeSearchResults->push_back(*mvp_iter);
					}

				}

				GetThreadTimes(GetCurrentThread(),&ftDummy,&ftDummy,&kernalEndTime,&userEndTime);

				t=((userEndTime.dwLowDateTime + kernalEndTime.dwLowDateTime) - (userStartTime.dwLowDateTime + kernalStartTime.dwLowDateTime)) / 10000000.00;

#else
#ifdef __GNUC__

				clock_t beginTime = times(NULL);

				if (searchMode == 0)
				{

					shared_ptr<CMVPIndex>  mvpIndex(new CMVPIndex());

					
					mvpIndex->setFileName(indexFileName);

					mvpIndex->setDataType(dataType);
					mvpIndex->setMetric(cmetric);
					mvptreeSearchResults = mvpIndex->search(&q);



					CMemMonitor::updateMem();

					averageMVPSearchCount[i] += mvptreeSearchResults->size();
				}
				else if (searchMode == 1)
				{

					shared_ptr< CSearchTask > searchTask (new CSearchTask(indexFileName,dataType,*cmetric));

					searchTask->searchWithCache(q, rootadd);
					averageMVPSearchCount[i] += searchTask->getResultSize();

				//	list<shared_ptr<CMetricData> >* searchResults = searchTask->search(q);
				//	
				//	CMemMonitor::updateMem();

				//	averageMVPSearchCount[i] += searchResults->size();

				//	list<shared_ptr<CMetricData> >::iterator mvp_iter = searchResults->begin();
				//	for(;mvp_iter!=searchResults->end();mvp_iter++)
				//	{
				//		mvptreeSearchResults->push_back(*mvp_iter);
				//	}

				}

				else 

				{

					shared_ptr<CExcludedMiddleForest>  ForestIndex(new CExcludedMiddleForest());

					ForestIndex->setFileName(indexFileName);

					ForestIndex->setDataType(dataType);

					ForestIndex->setMetric(cmetric);

					list<shared_ptr<CMetricData> >* searchResults = ForestIndex->search(&q);

					CMemMonitor::updateMem();

					averageMVPSearchCount[i] += searchResults->size();

					list<shared_ptr<CMetricData> >::iterator mvp_iter = searchResults->begin();
					for(;mvp_iter!=searchResults->end();mvp_iter++)
					{
						mvptreeSearchResults->push_back(*mvp_iter);
					}

				}

				clock_t endTime = times(NULL);

				t = (endTime-beginTime)/100.0;

				cout << "endTime - beginTime:" << t << endl;

#endif
#endif

				distanceFunctionCounterAfterSearch=dynamic_cast<CountedMetric*>(cmetric)->getCounter();

				mvpqueryTime[i] +=t;

				if(maxQueryTime[i]<t)

					maxQueryTime[i]=t;

				if(minQueryTime[i]>t)

					minQueryTime[i]=t;

				int d=distanceFunctionCounterAfterSearch-distanceFunctionCounterBeforSearch;



				mvpdistanceCalculatedTimes[i]+=d;

				if(maxDistanceCalculatedTimes[i]<d)

					maxDistanceCalculatedTimes[i]=d;

				if(minDistanceCalculatedTimes[i]>d)

					minDistanceCalculatedTimes[i]=d;


#ifdef __GNUC__
				if (peakWorkingSetSize[i] < CMemMonitor::maxWorkingSet)

					peakWorkingSetSize[i] = CMemMonitor::maxWorkingSet;

				avgpeakWorkingSetSize[i] += CMemMonitor::maxWorkingSet;



				if (peekmemoryUseSize[i] < CMemMonitor::maxMem)

					peekmemoryUseSize[i] = CMemMonitor::maxMem;

				avgpeekmemoryUseSize[i] += CMemMonitor::maxMem;



				if (peakpagefileusage[i] < CMemMonitor::maxPageUsing)

					peakpagefileusage[i] = CMemMonitor::maxPageUsing;

				avgpagefileusage[i] += CMemMonitor::maxPageUsing;
#else
#ifdef _WIN32
				if (peakWorkingSetSize[i] < CMemMonitor::maxWorkingSet)
					peakWorkingSetSize[i] = CMemMonitor::maxWorkingSet;
				avgpeakWorkingSetSize[i] += CMemMonitor::maxWorkingSet/1048576.0;

				if (peekmemoryUseSize[i] < CMemMonitor::maxMem)
					peekmemoryUseSize[i] = CMemMonitor::maxMem;
				avgpeekmemoryUseSize[i] += CMemMonitor::maxMem/1048576.0;

				if (peakpagefileusage[i] < CMemMonitor::maxPageUsing)
					peakpagefileusage[i] = CMemMonitor::maxPageUsing;
				avgpagefileusage[i] += CMemMonitor::maxPageUsing/1048576.0;
#endif
#endif

				//for verify
				if(verify)
				{
					CMetricDistance *metric=0;
					getMetricByType(metric,dataType);
					verifySearchResult(rawData,metric,q,mvptreeSearchResults,output);
				}

				count++;
				delete mvptreeSearchResults;
			}

#ifdef __GNUC__
			clock_t endTTime = times(NULL);
#endif

			//mvpqueryTime[i]/=count-1;

			//mvpdistanceCalculatedTimes[i]/=count-1;

			//averageMVPSearchCount[i] /= count-1;

			avgpagefileusage[i] /= count-1;

			avgpeekmemoryUseSize[i] /= count-1;

			avgpeakWorkingSetSize[i] /= count-1;

			strtrans << averageMVPSearchCount[i] << " " << (endTTime - beginTTime)/100.0 << " " << mvpqueryTime[i] << " ";
			//cout << __LINE__ <<" ss.size:" << strlen(strtrans.str().c_str()) << " " << strtrans.str() << endl;
			


#ifdef _WIN32
			peakWorkingSetSize[i] = peakWorkingSetSize[i]/1048576.0;
			peekmemoryUseSize[i] = peekmemoryUseSize[i]/1048576.0;
			peakpagefileusage[i] = peakpagefileusage[i]/1048576.0;
#else
#ifdef __GNUC__
			peakWorkingSetSize[i] = peakWorkingSetSize[i];
			peekmemoryUseSize[i] = peekmemoryUseSize[i];
			peakpagefileusage[i] = peakpagefileusage[i];

#endif
#endif

			//strtrans << dataBaseSize << " " << searchRadius << " " <<  mvpdistanceCalculatedTimes[i]  << " " << mvpqueryTime[i] << " " << averageMVPSearchCount[i] << " " << avgpeakWorkingSetSize[i] << " " << peakWorkingSetSize[i] << " " << avgpeekmemoryUseSize[i] << " " << peekmemoryUseSize[i] << endl;
			output << dataBaseSize << setw(15) << searchRadius << setw(15) <<  mvpdistanceCalculatedTimes[i]  << setw(20) << mvpqueryTime[i] << setw(15) << averageMVPSearchCount[i] << setw(20) << avgpeakWorkingSetSize[i] << setw(20) << peakWorkingSetSize[i] << setw(20) << avgpeekmemoryUseSize[i] << setw(20) << peekmemoryUseSize[i] << endl;

			

		}

		resultcontainer.append(strtrans.str().c_str());
		strtrans.str("");

		delete [] mvpqueryTime;
		delete [] maxQueryTime;
		delete [] minQueryTime;
		delete [] mvpdistanceCalculatedTimes;
		delete [] maxDistanceCalculatedTimes;
		delete [] minDistanceCalculatedTimes;
		delete [] averageMVPSearchCount;

		delete [] peakWorkingSetSize;
		delete [] peekmemoryUseSize;
		delete [] avgpagefileusage;
		delete [] peakpagefileusage;
		delete [] avgpeekmemoryUseSize;
		delete [] averageMVPSearchCount;

		delete(queryList);

	}
	else if(searchMode==3)
	{
		int queryStart = (firstQuery<0)?0:firstQuery;
		int queryEnd = (lastQuery>queryList->size())?queryList->size():lastQuery;

		vector<shared_ptr<CMetricData> > newQueryList;
		vector<shared_ptr<CMetricData> >::iterator ite = queryList->begin();
		newQueryList.insert(newQueryList.begin(),ite+queryStart,ite+queryEnd);
		nglobalMultithreadSearch(rawData,&newQueryList,dataType,indexFileName,verify,maxRadius,minRadius,step,cmetric,resultFileName,dataBaseSize,cacheHeight,maximumThreadsNumForSearching, resultcontainer);
	}
	else
	{
		cout << "search Mode== 4" << endl;
		cout << "maxT:" << maximumThreadsNumForSearching << endl;
		cout << "totalTs:" << totalSearchingThreadForChildPathSearchingTaskInThreadpool << endl;
		int queryStart=(firstQuery<0)?0:firstQuery;
		int queryEnd=(lastQuery>queryList->size())?queryList->size():lastQuery;

		vector<shared_ptr<CMetricData> > newQueryList;
		vector<shared_ptr<CMetricData> >::iterator ite=queryList->begin();
		newQueryList.insert(newQueryList.begin(),ite+queryStart,ite+queryEnd);

		multiThreadsInInterAndInner(&newQueryList,dataType,indexFileName,maxRadius,minRadius,step,resultFileName,dataBaseSize,cacheHeight,maximumThreadsNumForSearching,totalSearchingThreadForChildPathSearchingTaskInThreadpool, resultcontainer);
	}
}

//search mode 4
ThreadPoolManager tpm;

void* singletf(void* vpb)
{
	singlequerytask *sqt=(singlequerytask*)vpb;

#ifdef _WIN32
	FILETIME kernelStart,kernelEnd;
	FILETIME userStart,userEnd;
	FILETIME dummy;
#else
	clock_t begt,endt;
#endif

#ifdef _WIN32
	GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelStart,&userStart);
#else
	begt=times(NULL);
#endif
	sqt->searchTask->singleSearchWithThreadPool(*(sqt->rq),sqt->rootaddress);
#ifdef _WIN32
	GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelEnd,&userEnd);
	sqt->searchtime=(userEnd.dwLowDateTime+kernelEnd.dwLowDateTime-userStart.dwLowDateTime-kernelEnd.dwLowDateTime)/10000000.00;
#else
	endt=times(NULL);
	sqt->searchtime=(endt-begt)/100.0;
#endif

	sqt->resultnumber=sqt->searchTask->getResultSize();
	sqt->distancecount=sqt->cmetric->getCounter();

	sqt->searchTask.reset();
	sqt->rq.reset();

	return NULL;
}


void multiThreadsInInterAndInner(vector<shared_ptr<CMetricData> >* queryObjectsList,char *&dataType,char *&indexFileName,double maximumSearchRadius,double minimumSearchRadius,double step,char *&resultFileName,int dataSize,int cacheHeight,int totalSearchingThreadInThreadPool,int totalSearchingThreadForChildPathSearchingTaskInThreadpool, string &resultcontainer)
{
	fstream output(resultFileName,ios::out|ios::app);
	
	tpm.setMaxThreadNumber(totalSearchingThreadInThreadPool);
	tpm.setMaxThreadNumberForChildren(totalSearchingThreadForChildPathSearchingTaskInThreadpool);
	cout << "tt:" << totalSearchingThreadInThreadPool << endl;
	cout << "tc:" << totalSearchingThreadForChildPathSearchingTaskInThreadpool << endl;
	
	long rootaddress=0;
	ifstream in(indexFileName,ios::binary);
	in.seekg(-(long)sizeof(long),ios::end);
	in.read((char*)&rootaddress,sizeof(long));
	in.close();

	CMetricDistance *metric=0;
	if(strcmp(dataType,"vector")==0)
	{
		metric = new CEuclideanDistance;
	}
	else if(!strcmp(dataType,"spectra")) 
	{
		//metric = new CMSMSMetric;
	}
	else if(!strcmp(dataType,"string")) 
	{
		metric = new CEditDistance;
	}
	else if(!strcmp(dataType,"rna")) 
	{
		metric = new CRNAMetric ;
	}
	else if(!strcmp(dataType,"image")) 
	{
		metric = new CImageMetric ;	
	}
	else if(!strcmp(dataType,"dna")) 
	{
		metric = new CDNAMetric ;	
	}
	else if(!strcmp(dataType,"peptide"))
	{
		metric = new CPeptideMetric;
	}

	int runNumber=floor((maximumSearchRadius-minimumSearchRadius)/step)+1;

	double *resultCounter=new double[runNumber];
	int *distanceCounter=new int[runNumber];
	double *searchTime=new double[runNumber];

	for(int i=0;i<runNumber;i++)
	{
		distanceCounter[i]=0;
		resultCounter[i]=0;
		searchTime[i]=0;
	}

#ifdef _WIN32
		FILETIME kernelStart,kernelEnd;
		FILETIME userStart,userEnd;
		FILETIME dummy;
#else
		clock_t begt,endt,ibegt,iendt;
#endif

	mutex condition_mutex;
	condition_variable condition;
	
	strtrans << runNumber << " ";


	int qnum = queryObjectsList->size();
	
	tpm.run();
	for(int r=0;r<runNumber;r++)
	{
		int totalqueryobjectnumber=qnum;
		int variablequeryobjectnumber=0;
		list<tp_task*> tasklist;
		
		for(int i=0;i<qnum;i++)
		{
			shared_ptr<CountedMetric> cm(new CountedMetric(metric));
			singlequerytask *task=new singlequerytask;
			shared_ptr<CSearchTask> search(new CSearchTask(indexFileName,dataType,*cm));
			shared_ptr<CRangeQuery> rq(new CRangeQuery(minimumSearchRadius+step*r,queryObjectsList->at(i)));

			task->searchTask=search;
			task->rq=rq;
			task->cmetric=cm;
			task->rootaddress=rootaddress;
			task->searchtime=0;
			task->distancecount=0;
			task->resultnumber=0;

			tp_task *tpt=new tp_task;
			tpt->ifinner=0;
			tpt->totalroute=totalqueryobjectnumber;
			tpt->variableroutenumber=&variablequeryobjectnumber;
			tpt->cond=&condition;
			tpt->route_mutex=&condition_mutex;
			tpt->task=(void*)task;
			tpt->func=singletf;

			tasklist.push_back(tpt);
		}

#ifdef _WIN32
	GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelStart,&userStart);
#else
	begt=times(NULL);
#endif
		cout << "add task list to thread list " << endl;	
		tpm.addCompleteTask(&tasklist);
		{
			std::unique_lock<std::mutex> lk(condition_mutex);
			while(variablequeryobjectnumber<totalqueryobjectnumber)
				condition.wait(lk);
		}		
		cout << "after add task list to thread list " << endl;	
	#ifdef _WIN32
		GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelEnd,&userEnd);
	#else
		endt=times(NULL);
	#endif

		for(int i=0;i<totalqueryobjectnumber;i++)
		{
			tp_task *sqt=tasklist.back();
			tasklist.pop_back();

			resultCounter[r]+=((singlequerytask*)(sqt->task))->resultnumber;
			distanceCounter[r]+=((singlequerytask*)(sqt->task))->distancecount;
			searchTime[r]+=((singlequerytask*)(sqt->task))->searchtime;
			delete (singlequerytask*)(sqt->task);
			delete sqt;
		}

		//strtrans << (minimumSearchRadius + step * r) << " " << resultCounter[r]/qnum << " " << (endt - begt)/100.0 << " " << (searchTime[r] / qnum) << " ";
		strtrans << (minimumSearchRadius + step * r) << " " << resultCounter[r] << " " << (endt - begt)/100.0 << " " << searchTime[r] << " ";
	}

	cout << "tmp.stop" << endl;	
	tpm.stop();

	for(int i=0;i<runNumber;i++)
	{
		output<<dataSize<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<maximumSearchRadius<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(distanceCounter[i]/(qnum))<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(searchTime[i])<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(((double)(resultCounter[i]))/(qnum))<<endl;
	}
#ifdef _WIN32
	output<<" total queryObject number:"<<qnum<<"total thread number is:"<<totalSearchingThreadInThreadPool<<" child thread number is:"<<totalSearchingThreadForChildPathSearchingTaskInThreadpool<<" total search time:"<<(kernelEnd.dwLowDateTime+userEnd.dwLowDateTime-kernelStart.dwLowDateTime-userStart.dwLowDateTime)/10000000.00<<" cache height:"<<cacheHeight<<endl;
	output<<"======================================================================================================"<<endl;
#else
	output<<" total queryObject number:"<<qnum<<"total thread number is:"<<totalSearchingThreadInThreadPool<<" child thread number is:"<<totalSearchingThreadForChildPathSearchingTaskInThreadpool<<" total search time:"<<(endt-begt)/100.0<<" cache height:"<<cacheHeight<<endl;
	output<<"======================================================================================================"<<endl;
#endif

	resultcontainer.append(strtrans.str().c_str());
	strtrans.str("");

	output.close();
}

//search mode 3
void nglobalMultithreadSearch(vector<shared_ptr<CMetricData> > *rawData,vector<shared_ptr<CMetricData> > *queryObjectsList,char *dataType,char *indexFileName,bool verify,double maxRadius,double minRadius,double step,CountedMetric *&cmetric, char *resultFileName,int dataBaseSize,int cacheHeight,int threadNum, string &resultcontainer)
{
	ofstream output(resultFileName,ios::out|ios::app);
	
	int runNumber=floor((maxRadius-minRadius)/step)+1;
	
	double *resultCounter=new double[runNumber];
	int *distanceCounter=new int[runNumber];
	double *searchTime=new double[runNumber];
	for(int i=0;i<runNumber;i++)
	{
		distanceCounter[i]=0;
		resultCounter[i]=0;
		searchTime[i]=0;
	}

	CMetricDistance *metric=0;
	CountedMetric *cm=0;

	long rootaddress=0;
	ifstream in(indexFileName,ios::binary);
	in.seekg(-(int)sizeof(long),ios::end);
	in.read((char*)&rootaddress,sizeof(long));
	in.close();

	strtrans << runNumber << " ";
	//cout << __LINE__ <<" ss.size:" << strlen(strtrans.str().c_str()) << " " << strtrans.str() << endl;


	vector<pblock*> task;
	for(int i=0;i<runNumber;i++)
	{
		if(strcmp(dataType,"vector")==0)
		{
			metric = new CEuclideanDistance;
		}
		else if(!strcmp(dataType,"spectra")) 
		{
			//metric = new CMSMSMetric;
		}
		else if(!strcmp(dataType,"string")) 
		{
			metric = new CEditDistance;
		}
		else if(!strcmp(dataType,"rna")) 
		{
			metric = new CRNAMetric ;
		}
		else if(!strcmp(dataType,"image")) 
		{
			metric = new CImageMetric ;	
		}
		else if(!strcmp(dataType,"dna")) 
		{
			metric = new CDNAMetric ;	
		}
		else if(!strcmp(dataType,"peptide"))
		{
			metric = new CPeptideMetric;
		}

		cm=new CountedMetric(metric);

		pblock *pb=new pblock(gthread_num_mutex,rawData);
		{
			pb->dataType=dataType;
			pb->metric=cm;
			pb->rootaddress=rootaddress;
			pb->newIndexName=indexFileName;
			pb->output=&output;
			pb->queryObjectList=queryObjectsList;
			pb->resultCounter=resultCounter;
			pb->distanceCounter=distanceCounter;
			pb->searchTime=searchTime;
			pb->threadNum=threadNum;

			//for verify
			pb->rawData = rawData;
			pb->verify = verify;
		}

		task.push_back(pb);
	}
	
	thread *threadList=new thread[runNumber];
	double *totaltime = new double[runNumber];

#ifdef _WIN32

	FILETIME kernelStart,kernelEnd;

	FILETIME userStart,userEnd;

	FILETIME dummy;

#else
#ifdef __GNUC__
	clock_t begt,endt;
#endif
#endif

#ifdef _WIN32

		GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelStart,&userStart);

#else
#ifdef __GNUC__

		begt=times(NULL);

#endif
#endif
	int qnum = queryObjectsList->size();


	for(int i=0;i<runNumber;i++)
	{
#ifdef _WIN32

		GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelStart,&userStart);

#else
#ifdef __GNUC__

		clock_t sbegt=times(NULL);

#endif
#endif

		
		pblock *pb=task.at(i);

		double searchRadius=minRadius+step*i;

		pb->num=i;
		pb->searchRadius=searchRadius;

		
		globalThreadFuncWithThreadPool(pb);//thread pool
		//globalThreadFuncForSingleSearchObjectUnlimitedThreadNumber(pb);//search thread task for every search radius with unlimited thread number -direact parallel
		//globalThreadFuncForSingleSearchObject(pb);//search thread task for every search radius with limited thread number 

#ifdef _WIN32

		GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelEnd,&userEnd);

#else
#ifdef __GNUC__

		clock_t sendt=times(NULL);
#endif
#endif		

		totaltime[i] = (sendt - sbegt) / 100.0;
	}

#ifdef _WIN32

		GetThreadTimes(GetCurrentThread(),&dummy,&dummy,&kernelEnd,&userEnd);

#else
#ifdef __GNUC__

		endt=times(NULL);
#endif
#endif	

	for(int i=0;i<runNumber;i++)
	{
		//threadList[i].join();
	}
	//showPromptInformation(__LINE__);
	delete [] threadList;
	for(int i=0;i<runNumber;i++)
	{
		//strtrans << (minRadius + step * i) << " " << resultCounter[i]/qnum << " " << totaltime[i] << " " << (searchTime[i]/(qnum)) << " ";
		strtrans << (minRadius + step * i) << " " << resultCounter[i] << " " << totaltime[i] << " " << searchTime[i] << " ";

		output<<dataBaseSize
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<(minRadius+step*i)
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(distanceCounter[i]/(qnum))
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(searchTime[i]/(qnum))
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(15)<<(resultCounter[i]/(qnum))
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<"****"
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<"****"
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<"****"
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<"****"
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(25)<<"****"
		<<setiosflags(ios::right)<<setiosflags(ios::fixed)<<setw(20)<<"****"<<endl;
	}


	


#ifdef _WIN32

		output<<"searchMode:"<<3<<" query number:"<<qnum<<" total queryObject number:"<<runNumber*qnum<<" threadNum:"<<threadNum<<" total search time:"<<(userEnd.dwLowDateTime+kernelEnd.dwLowDateTime-userStart.dwLowDateTime-kernelEnd.dwLowDateTime)/10000000.00<<" cache height:"<<cacheHeight<<endl;

#else
#ifdef __GNUC__

		output<<"searchMode:"<<3<<" query number:"<<qnum<<" total queryObject number:"<<runNumber*qnum<<" threadNum:"<<threadNum<<" total search time:"<<(endt-begt)/100.0<<" cache height:"<<cacheHeight<<endl;

#endif
#endif
	
	output<<"======================================================================================================"<<endl;

	resultcontainer.append(strtrans.str().c_str());
	strtrans.str("");
	output.close();
	//showPromptInformation(__LINE__);
	for(int i=0;i<task.size();i++)
		delete task.at(i);
	//showPromptInformation(__LINE__);
}


/**@function to verify the search result is or not right
* @brief to verify the search result is or not right
* @param rawData raw data set. 
* @param metric to calculate the distance between two objects.
* @param q a query object.
* @param queryResultList mvp search result.
* @param output out stream.
* @post when there is an error, message will export to result file; if there is no error, nothing will export to result file.
*/
void verifySearchResult(vector<shared_ptr<CMetricData> > *&rawData,CMetricDistance *metric,CRangeQuery q, list<shared_ptr<CMetricData> > *queryResultList,ofstream &output)
{
	vector<shared_ptr<CMetricData> > linerSearchResults ;

	//linear traversal to get a result which must be true
	for (vector<CMetricData*>::size_type ii=0;ii!=rawData->size();ii++)
	{

		double objectDistance = metric->getDistance(q.getQueryObject().get(),rawData->at(ii).get());

		if(objectDistance<=q.getRadius())
			linerSearchResults.push_back(rawData->at(ii));

	}

	//check two results'length. if length is not same, it must be an error.
	if(queryResultList->size()!=linerSearchResults.size())
	{
		output<<"radius="<<q.getRadius()<<"  has an error:  "<<"mvptreeSearchResults->size()!=linerSearchResults.size() : "<<queryResultList->size()<<"   "<<linerSearchResults.size()<<endl;
	}
					
	list<shared_ptr<CMetricData> >::iterator mvp_iter = queryResultList->begin();

					
	for(;mvp_iter!=queryResultList->end();mvp_iter++)
	{
		vector<shared_ptr<CMetricData> >::size_type jj=0 ;
		for(;jj!=linerSearchResults.size();jj++)//"jj=linerSearchResults.size()" has only one situation which this loop has caught out, but failed to find the result
		{
			if(metric->getDistance((*mvp_iter).get(),linerSearchResults[jj].get())==0.0) break;//get out of loop right now after finding out the result
		}
		if(jj==linerSearchResults.size()) break;//means failing to find the result
	}
	if(mvp_iter!=queryResultList->end()) //get out of loop in the halfway because a result hasn't found.
		output<<"when radius is "<<q.getRadius()<<"  has an error!"<<endl;
}