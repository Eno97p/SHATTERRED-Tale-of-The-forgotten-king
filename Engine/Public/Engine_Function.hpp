#ifndef Engine_Function_h__
#define Engine_Function_h__



#include<fstream>
#include<tuple>
//#include<optional>
using namespace std;



namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}


	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		iRefCnt = { 0 };

		if (nullptr != pInstance)		
			iRefCnt = pInstance->AddRef();				

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int		iRefCnt = { 0 };

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if (0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}


	template<typename T>
	void Safe_physX_Release(T& pInstance)
	{

		if (nullptr != pInstance)
		{
			
			pInstance->release();
			pInstance = nullptr;
		}



	}
	template<typename T>
	void Safe_Vehicle_Release(T& pInstance)
	{

		if (nullptr != pInstance)
		{

			pInstance->destroy();
			pInstance = nullptr;
		}

	}




	template<typename T>
	void writeVectorToBinary(std::vector<T>& vec, std::ostream& os) {
		// 벡터의 요소 개수를 이진 파일에 쓰기
		size_t size = vec.size();
		os.write((char*)(&size), sizeof(size_t));
		// 벡터의 각 요소를 이진 파일에 쓰기
		os.write((char*)(vec.data()), sizeof(T) * size);
	}

	template<typename T>
	void readVectorFromBinary(std::istream& is, std::vector<T>& vec) {
		// 벡터의 요소 개수를 이진 파일에서 읽어오기
		size_t size;
		is.read((char*)(&size), sizeof(size_t));
		// 벡터의 크기를 조정하여 메모리를 할당
		vec.resize(size);
		// 벡터의 요소를 이진 파일에서 읽어오기
		is.read((char*)(vec.data()), sizeof(T) * size);
	}



	//파일 확장자 자르기

	inline wstring RemoveExtension(const wstring& filePath)
	{
		size_t lastDot = filePath.find_last_of(L'.');
		if (lastDot == std::wstring::npos) {
			return filePath; // 확장자가 없는 경우 원래 문자열 반환
		}
		return filePath.substr(0, lastDot);
	}




	template<typename T>
	void Save_Data_Extraction(std::ofstream& outFile, const T& data)
	{
		//만약 wstring이 들어온 경우 
		if constexpr (is_same_v<T, wstring>)
		{
			//길이를 먼저 저장 하고 그 다음 문자를 저장
			size_t size = data.size();
			outFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
			outFile.write(reinterpret_cast<const char*>(data.c_str()), size * sizeof(wchar_t));

		}
		else
		{	//문자열이 안 들어왔을 경우
			outFile.write(reinterpret_cast<const char*>(&data), sizeof(T));
		}
		//사이즈 테스트용 코드 
		//int tmp = 0;
		//tmp = sizeof(T);


	}

	template<typename T1, typename... Types>
	void Save_Data(T1 FilePath,bool IsCover, Types... arg)
	{
		std::wstring filePathWithoutExtension = RemoveExtension(FilePath);
		std::wstring fullPath = filePathWithoutExtension + L".bin";

		ios_base::openmode mode = ios::out | ios::binary;
		if(IsCover)
			mode |= ios::trunc;
	
	


		std::ofstream outFile(fullPath, mode/*| std::ios::app*/);

		if (outFile.is_open())
		{
			//Save_Data(FilePath, 1,2,3,4,5,6) 이렇게 호출 됐다면 
			((Save_Data_Extraction(outFile, arg)), ...);	//폴드 표현식
			//Save_Data_Extraction는 
			//Save_Data_Extraction(outFile, 1);
			//Save_Data_Extraction(outFile, 2);
			//Save_Data_Extraction(outFile, 3);
			//Save_Data_Extraction(outFile, 4);
			//Save_Data_Extraction(outFile, 5);
			//Save_Data_Extraction(outFile, 6);
			//이렇게 6번호출된다.

			//이렇게 해도 됨
			//((outFile.write(reinterpret_cast<const char*>(&arg), sizeof(arg))), ...);
		}


		outFile.close();
	}






	template<typename T>
	void Load_Data_Extraction(std::ifstream& inFile, T& data)
	{
		if constexpr (is_same_v<T, wstring>)	//들어온 타입이 wstring이라면 
		{
			size_t size = 0;
			inFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));		//사이즈를 읽어오기
			wchar_t* buffer = new wchar_t[size + 1];		//null문자 고려해서 +1 만큼 할당
			ZeroMemory(buffer, sizeof(wchar_t) * (size + 1));
			inFile.read(reinterpret_cast<char*>(buffer), size * sizeof(wchar_t));	//문자열을 읽어오기
			data = buffer;		//문자열을 tuple에 넣어주기
			delete[] buffer;
		}
		else
			inFile.read(reinterpret_cast<char*>(&data), sizeof(T));
	}

	// 파일에서 데이터를 읽어오는 함수 (재귀적 호출)
	template<_uint iArgCnt = 0, typename... Types>
	void Load_Data_Recursive(ifstream& inFile, tuple<Types...>& pdata)
	{

		//컴파일 타임에 탈출 조건을 준 이유는
		// Load_Data()를 호출 할 때 명시적으로 타입과 갯수를 주고 
		//

		const int tmp = sizeof...(Types);	//들어온 타입의 갯수   // sizeof...(arg): 파라메터 팩의 갯수를 반환한다.
		if constexpr (iArgCnt < tmp)							//조건식이 참일 때만 컴파일시간에 코드를 생성함 c++17부터 지원하는 기능
		{														//재귀 탈출 조건을 컴파일시간에 체크한다
																//그러므로 상수만 비교대상이 될 수 있다.
			Load_Data_Extraction(inFile, get<iArgCnt>(pdata));	//튜플의 iArgCnt번째에 인자 값을 채운다
			Load_Data_Recursive<iArgCnt + 1>(inFile, pdata);	//재귀적으로 호출한다.
		}
	}

	// 파일에서 데이터를 읽어오는 함수 (타입 지정) 명시적으로 어떤 타입인지 지정해줘야 함
	template<typename... Types>	//타입이 지정이 되었다면, 
	//튜플의 반환값을 지정된 타입으로 반환시킨다
	std::optional<tuple<Types...>> Load_Data(const wstring& FilePath)	//optional 파일이 유효한지 확인하기위함
	{
		std::wstring filePathWithoutExtension = RemoveExtension(FilePath);
		wstring fullPath = filePathWithoutExtension + L".bin";
		ifstream inFile(fullPath, ios::in | ios::binary);
		//튜플을 만든다.

		if (!inFile.is_open())
		{
			// 파일이 없으면 nullopt 반환
			return std::nullopt;
		}


		tuple<Types...> data;
		//if (inFile.is_open())
		{
			//튜플에 값을 채운다.(Load_Data_Recursive 이 함수 내에서 재귀적으로 채우는 거임 )
			//Load_Data_Recursive()이 함수를 호출할 때 원래는 Load_Data_Recursive<?,?>이런식으로 호출해야 하지만
			//Load_Data()를 호출할 때 명시적으로 타입을 지정해줬기 때문에
			//컴파일러가 알아서 타입을 추론하고, 암묵적으로 Load_Data_Recursive함수에 인수를 넣어준다.
			Load_Data_Recursive(inFile, data);
		}

		inFile.close();

		return data;
	}










//txt 저장 함수
	template<typename T1, typename... Types>
	void Save_Data_Text(T1 FilePath, Types... arg)
	{
		std::wstring filePathWithoutExtension = RemoveExtension(FilePath);
		std::wstring fullPath = filePathWithoutExtension + L".txt";


		std::wofstream outFile(fullPath);
		if (outFile.is_open())
		{
			((outFile << arg << std::endl), ...);
		}


		outFile.close();
	}








	template<typename T>
	void Load_Data_Extraction_Text(std::wifstream& inFile, T& data)
	{
		std::wstring line;
		if (std::getline(inFile, line))
		{
			std::wstringstream ss(line);
			ss >> data;
		}
	}

	template<std::size_t iArgCnt = 0, typename... Types>
	void Load_Data_Recursive_Text(std::wifstream& inFile, std::tuple<Types...>& pdata)
	{
		constexpr std::size_t tmp = sizeof...(Types);
		if constexpr (iArgCnt < tmp)
		{
			Load_Data_Extraction_Text(inFile, std::get<iArgCnt>(pdata));
			Load_Data_Recursive_Text<iArgCnt + 1>(inFile, pdata);
		}
	}

	template<typename... Types>
	std::tuple<Types...> Load_Data_Text(const std::wstring& FilePath)
	{
		std::wstring filePathWithoutExtension = RemoveExtension(FilePath);
		std::wstring fullPath = filePathWithoutExtension + L".txt";
		std::wifstream inFile(fullPath);
		std::tuple<Types...> data;

		if (inFile.is_open())
		{
			Load_Data_Recursive_Text(inFile, data);
		}

		inFile.close();

		return data;
	}













	
	template<typename T>
	T RandomNum(T RandomRangeX, T RandomRangeY)
	{
		random_device randomDevice;
		mt19937_64 Gen(randomDevice());

		uniform_real_distribution<T> RandomDist(RandomRangeX, RandomRangeY);
		T RandomValue = RandomDist(Gen);

		return RandomValue;



	}
	





	
}

#endif // Engine_Function_h__
