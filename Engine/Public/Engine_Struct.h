#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	enum class eKEY_STATE
	{
		NONE,
		TAP, //누를떄
		HOLD,//누르고 있을때
		AWAY, // 똇을때
		KEY_END,
	};
	struct tKeyInfo
	{
		eKEY_STATE	eState; //키의 상태값
		bool		bPrevPush;	//이전프레임에 눌렀는지 안눌렀는지
	};







	/* 엔진을 초기화하기위해 필요한 데이터 여러개를 묶었다. */
	typedef struct ENGINE_DESC
	{
		bool			isWindowed;
		HWND			hWnd;
		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
	}ENGINE_DESC;

	typedef struct LIGHT_DESC
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOTLIGHT, TYPE_END };

		TYPE		eType = { TYPE_END };				

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;

		//For Spotlight
		float		innerAngle;
		float		outerAngle;


		/* 빛의 기본적인 색상. */
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHT_DESC;

	typedef struct DECAL_DESC
	{
		XMMATRIX vWorldInv;
	}DECAL_DESC;

	//0 0 1 1
	//1 1 0 1

	//0 0 0 1


	//{
	//	XMFLOAT4	vDiffuse;
	//}D3DMATERIAL9

	typedef struct MESH_MATERIAL
	{
		class CTexture*		MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	typedef struct KEYFRAME
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	}KEYFRAME;

	typedef struct ENGINE_DLL VTXPOS
	{
		XMFLOAT3		vPosition;		

		static const unsigned int		iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];

	}VTXPOS;


	typedef struct ENGINE_DLL VTXPHYSX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vColor;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXPHYSX;





	typedef struct ENGINE_DLL VTXPOINT
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOINT;





	typedef struct ENGINE_DLL  VTXPOSTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXPOSTEX;

	 
	typedef struct ENGINE_DLL VTXCUBE
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int		iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];

	}VTXCUBE;

	typedef struct ENGINE_DLL VTXNORTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int		iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];

	}VTXNORTEX;

	typedef struct ENGINE_DLL VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	}VTXMESH;

	typedef struct ENGINE_DLL VTXCLOUD
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int		iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];

	}VTXCLOUD;

	typedef struct ENGINE_DLL VTXANIMMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int		iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];

	}VTXANIMMESH;

	typedef struct  ENGINE_DLL VTXMATRIX
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT2			vLifeTime;
		_float			   vGravity;
		//_float padding;  // 4바이트 패딩 추가
		//void setToDefault()		//Test
		//{
		//	vRight = { 0,0,0,0 };
		//	vUp = { 0,0,0,0 };
		//	vLook = { 0,0,0,0 };
		//	vTranslation = { 0,0,0,0 };
		//	vLifeTime = { 0,0 };
		//	vGravity = 0;
		//}

	}VTXMATRIX;


	typedef struct ENGINE_DLL VTXPARTICLE
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT2			vLifeTime;
	};





	typedef struct ENGINE_DLL VTXINSTANCE_RECT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL VTXINSTANCE_POINT
	{
		static const unsigned int		iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL VTXINSTANCE_MESH
	{
		static const unsigned int		iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[8];
	}VTXINSTANCE_MESH;
		
	
	typedef struct ENGINE_DLL VTXINSTANCE_MAPELEMENT
	{
		static const unsigned int		iNumElements = { 12 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[12];
	}VTXINSTANCE_MAPELEMENT;

	typedef struct ENGINE_DLL VTXPARTICLE_MESH
	{
		static const unsigned int		iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[9];
	};

	typedef struct ENGINE_DLL SwordTrailVertex
	{
		XMFLOAT3 Zero;
		XMFLOAT3 One;
		XMFLOAT3 Two;
		XMFLOAT3 Three;

		XMFLOAT2 texCoord0;
		XMFLOAT2 texCoord1;
		XMFLOAT2 texCoord2;
		XMFLOAT2 texCoord3;

		_float2 lifetime;
		static const unsigned int		iNumElements = { 9 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[9];
	};


	typedef struct ENGINE_DLL  VTXELECTRON
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
		XMFLOAT3		vStartpos;
		XMFLOAT3		vEndpos;
		XMFLOAT2		vLifeTime;
		_float			fThickness;
		_float			fAmplitude;
		_int			iNumSegments;
		static const unsigned int		iNumElements = { 8 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[8];
	};


	typedef struct ENGINE_DLL tEvent //이벤트 매니저 파라미터
	{
		eEVENT_TYPE		eEven;
		DWORD_PTR		lParam;
		DWORD_PTR		wParam;
		DWORD_PTR		pParam;
		DWORD_PTR		eParam;
		DWORD_PTR		fParam;
	}tEvent;

	struct ENGINE_DLL MATRIX4
	{
		union
		{
			struct 
			{
				_float	_11, _12, _13, _14;
				_float	_21, _22, _23, _24;
				_float	_31, _32, _33, _34;
				_float	_41, _42, _43, _44;

			};
			struct
			{
				float f[4][4];
			};
		};
	};

	namespace Vehicle
	{

	#define MAX_NUM_WHEELS 4


		//차량을 만드는데 필요한 기본적인 정보들
		struct ENGINE_DLL VehicleBeginParams
		{
		public:
			//디폴트로 초기화 필요하다면 초기화
			PX_FORCE_INLINE VehicleBeginParams()
				: mass(1000.0f), dimensions(1.0f, 1.0f, 1.0f), numWheels(4) , maxSteeringAngle(PxPi / 4)
			{
				for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					wheelRadius[i] = 0.5f;
			}
			PX_FORCE_INLINE VehicleBeginParams(PxReal _mass, PxVec3 _dimensions, PxU32 _numWheels, PxReal _wheelRadius, PxReal _maxSteeringAngle)
				: mass(_mass), dimensions(_dimensions), numWheels(_numWheels), maxSteeringAngle(_maxSteeringAngle)
			{
				for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					wheelRadius[i] = _wheelRadius;
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleBeginParams)); }
		

			PxReal mass;			//차량의 질량
			PxVec3 dimensions;		//차량의 크기
			PxU32 numWheels;		//차량의 바퀴 갯수
			PxReal wheelRadius[MAX_NUM_WHEELS];		//바퀴의 반지름
			PxReal maxSteeringAngle;  // 최대 조향 각도 추가
		};

		struct ENGINE_DLL VehicleBeginState
		{
			public:
				//디폴트로 초기화 필요하다면 초기화
				PX_FORCE_INLINE VehicleBeginState()
					: transform(PxTransform(PxIdentity)), linearVelocity(PxVec3(0)), angularVelocity(PxVec3(0))	
				{
				}
				PX_FORCE_INLINE VehicleBeginState(PxTransform _transform, PxVec3 _linearVelocity, PxVec3 _angularVelocity)
					: transform(_transform), linearVelocity(_linearVelocity), angularVelocity(_angularVelocity)
				{
				}

				PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleBeginState));	}
				PX_FORCE_INLINE void setData(const VehicleBeginState& other)
				{
					transform = other.transform;
					linearVelocity = other.linearVelocity;
					angularVelocity = other.angularVelocity;
				}


				PxTransform transform;		//차량의 위치
				PxVec3 linearVelocity;		//차량의 선속도
				PxVec3 angularVelocity;		//차량의 각속도
		};

		struct ENGINE_DLL VehicleMiddleParams
		{
		public:
			PX_FORCE_INLINE VehicleMiddleParams()
				: engineTorque(1000.0f), brakeTorque(1500.0f), steeringAngle(0.0f), steeringSpeed(PxPi / 2) , MaxSteeringAngle(PxPi / 4)
			{
			}
			PX_FORCE_INLINE VehicleMiddleParams(PxReal _engineTorque, PxReal _brakeTorque, PxReal _steeringAngle, PxReal _steeringSpeed, PxReal _maxsteeringAngle)
				: engineTorque(_engineTorque), brakeTorque(_brakeTorque), steeringAngle(_steeringAngle), steeringSpeed(_steeringSpeed), MaxSteeringAngle(_maxsteeringAngle)
			{
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleMiddleParams)); }



			PxReal engineTorque;		//엔진 토크
			PxReal brakeTorque;			//브레이크 토크
			PxReal steeringAngle;		//핸들 각도
			PxReal MaxSteeringAngle;	//최대 핸들 각도
			PxReal steeringSpeed;		//핸들 속도
		};

		struct ENGINE_DLL VehicleMiddleState
		{
			public:
				PX_FORCE_INLINE VehicleMiddleState()
				{
					PxMemZero(wheelSpeeds, sizeof(PxReal) * MAX_NUM_WHEELS);
					PxMemZero(suspensionCompression, sizeof(PxReal) * MAX_NUM_WHEELS);
					PxMemZero(&bodyRotation, sizeof(PxQuat));
				}
				PX_FORCE_INLINE VehicleMiddleState(PxReal _wheelSpeeds[MAX_NUM_WHEELS], PxReal _suspensionCompression[MAX_NUM_WHEELS], const PxQuat& BodyRotation  )
				{
					for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					{
						wheelSpeeds[i] = _wheelSpeeds[i];
						suspensionCompression[i] = _suspensionCompression[i];
					}
					bodyRotation = BodyRotation;
				}
				PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleMiddleState)); }
				PX_FORCE_INLINE void setData(const VehicleMiddleState& other)
				{
					for (PxU32 i = 0; i < MAX_NUM_WHEELS; i++)
					{
						wheelSpeeds[i] = other.wheelSpeeds[i];
						suspensionCompression[i] = other.suspensionCompression[i];
					}
					bodyRotation = other.bodyRotation;
				}

				PxReal wheelSpeeds[MAX_NUM_WHEELS];				//바퀴 속도
				PxReal suspensionCompression[MAX_NUM_WHEELS];	//서스펜션 압축
				PxQuat bodyRotation;							//바디 회전
		};

		struct ENGINE_DLL VehicleEndParams
		{
		public:

			PX_FORCE_INLINE VehicleEndParams()
				: maxSpeed(100.0f)
			{
			}

			PX_FORCE_INLINE void setToDefault()	{ PxMemZero(this, sizeof(VehicleEndParams)); }


			PxReal maxSpeed;		//최대 속도
		};

		struct ENGINE_DLL VehicleEndState
		{
		public:
			PX_FORCE_INLINE VehicleEndState()
				: finalPose(PxTransform(PxIdentity)), finalVelocity(PxVec3(0)), finalAngularVelocity(PxVec3(0))
			{
			}
			PX_FORCE_INLINE void setToDefault()	{
				PxMemZero(this, sizeof(VehicleEndState));
				finalPose = PxTransform(PxIdentity);
				finalVelocity = PxVec3(0);
				finalAngularVelocity = PxVec3(0);
			}
			PX_FORCE_INLINE void setData(const VehicleEndState& other)
			{
				finalPose = other.finalPose;
				finalVelocity = other.finalVelocity;
				finalAngularVelocity = other.finalAngularVelocity;
			}



			PxTransform finalPose;	//최종 위치
			PxVec3 finalVelocity;	//최종 속도
			PxVec3 finalAngularVelocity;  // 최종 각속도

		};



		struct ENGINE_DLL DirectDrivetrainState
		{
			PxReal directDriveThrottleResponseStates[PxVehicleLimits::eMAX_NB_WHEELS];

			PX_FORCE_INLINE void setToDefault()
			{
				PxMemZero(this, sizeof(DirectDrivetrainState));
			}
		};

		struct ENGINE_DLL  DirectDrivetrainParams
		{
			PxVehicleDirectDriveThrottleCommandResponseParams directDriveThrottleResponseParams;

			DirectDrivetrainParams transformAndScale(const PxVehicleFrame& srcFrame,
				const PxVehicleFrame& trgFrame,
				const PxVehicleScale& srcScale,
				const PxVehicleScale& trgScale) const
			{
				DirectDrivetrainParams r = *this;
				r.directDriveThrottleResponseParams = directDriveThrottleResponseParams.transformAndScale(srcFrame, trgFrame, srcScale, trgScale);
				return r;
			};

			PX_FORCE_INLINE bool isValid(const PxVehicleAxleDescription& axleDesc) const
			{
				if (!directDriveThrottleResponseParams.isValid(axleDesc))
					return false;

				return true;
			}
		};

		struct ENGINE_DLL WheelConfiguration
		{
			WheelConfiguration(_uint _frontWheels, _uint _rearWheels)
				: frontWheels(_frontWheels), rearWheels(_rearWheels)
			{
			}

			_uint frontWheels;
			_uint rearWheels;
		};


		struct ENGINE_DLL AxleDescription
		{
			PxU32 AxleCount;		//축의 갯수
			PxU32 WheelCounts[PxVehicleLimits::eMAX_NB_AXLES];	//축당 바퀴의 갯수
			PxU32 WheelIds[PxVehicleLimits::eMAX_NB_AXLES][PxVehicleLimits::eMAX_NB_WHEELS];	//축당 바퀴의 아이디
		};

		struct ENGINE_DLL VehicleFrame
		{
			PxU32 lngAxis;		//길이축
			PxU32 latAxis;		//폭축
			PxU32 vrtAxis;		//높이축
		};
		struct ENGINE_DLL VehicleScale
		{
			_float scale;		//스케일
		};
		struct ENGINE_DLL VehicleRigidBody 
		{
			_float mass;		//질량
			PxVec3 moi;			//관성모멘트

		};
		struct ENGINE_DLL VehicleSuspensionStateCalculation
		{
			_uint JounceCalculationType;	//Jounce 계산 타입
			_bool LimitSuspensionExpansionVelocity; //서스펜션 확장 속도 제한

		};
		struct ENGINE_DLL VehicleBrakeCommandResponse
		{
			PxVehicleCommandNonLinearResponseParams nonlinearResponse;
			PxReal wheelResponseMultipliers[PxVehicleLimits::eMAX_NB_WHEELS];	//바퀴 반응 멀티플라이어
			PxU32 numWheelsMulipliers;	//바퀴 멀티플라이어 갯수
			PxF32 maxResponse;		//최대 반응

		};
		struct ENGINE_DLL VehicleAckermann
		{
			PxU32 wheelIds[2];
			PxReal wheelBase;
			PxReal trackWidth;
			PxReal strength;
		};
		struct ENGINE_DLL VehicleSuspension
		{
			PxU32 wheelId;
			PxTransform suspensionAttachment;	//서스펜션 부착
			PxVec3 suspensionTravelDir;		//서스펜션 이동 방향
			PxReal suspensionTravelDist;		//서스펜션 이동 거리
			PxTransform wheelAttachment; //바퀴 부착
		};
		struct ENGINE_DLL VehicleSuspensionCompiance
		{
			PxVehicleFixedSizeLookupTable<PxReal, 3> wheelToeAngle;
			PxVehicleFixedSizeLookupTable<PxReal, 3> wheelCamberAngle;
			PxVehicleFixedSizeLookupTable<PxVec3, 3> suspForceAppPoint;
			PxVehicleFixedSizeLookupTable<PxVec3, 3> tireForceAppPoint;

		};
		struct ENGINE_DLL VehicleSuspensionForce
		{
			PxReal stiffness;	//스프링 상수
			PxReal damping;		//감쇠 상수
			PxReal sprungMass;	//스프링 질량
		};
		struct ENGINE_DLL VehicleTireForce
		{
			PxReal latStiffX; //측면 강성
			PxReal latStiffY; //측면 강성
			PxReal longStiff; //길이 강성
			PxReal camberStiff; //캠버 강성
			PxReal restLoad; //잔존하중
			PxReal frictionVsSlip[3][2]; //마찰 대 슬립
			PxReal loadFilter[2][2]; //로드 필터
		};
		struct ENGINE_DLL VehicleWheel
		{
			PxReal radius;	//반지름
			PxReal halfWidth; //반폭
			PxReal mass; //질량
			PxReal moi; //관성모멘트
			PxReal dampingRate; //감쇠율
		};


		struct ENGINE_DLL BaseVehicleDesc
		{
			PX_FORCE_INLINE void setToDefault()
			{
				PxMemZero(this, sizeof(BaseVehicleDesc));
			}


			AxleDescription						axleDesc;
			VehicleFrame						frameDesc;
			VehicleScale						scaleDesc;
			VehicleRigidBody					rigidBodyDesc;
			VehicleSuspensionStateCalculation	suspensionStateCalculationDesc;
			VehicleBrakeCommandResponse			brakeCommandResponseDesc[2];
			VehicleBrakeCommandResponse			steerCommandResponseDesc;
			VehicleBrakeCommandResponse 		throttleCommandResponseDesc;
			VehicleAckermann					ackermannDesc;
			VehicleSuspension					suspensionDesc[PxVehicleLimits::eMAX_NB_WHEELS];
			VehicleSuspensionCompiance			suspensionCompianceDesc[PxVehicleLimits::eMAX_NB_WHEELS];
			VehicleSuspensionForce				suspensionForceDesc[PxVehicleLimits::eMAX_NB_WHEELS];
			VehicleTireForce					tireForceDesc[PxVehicleLimits::eMAX_NB_WHEELS];
			VehicleWheel						wheelDesc[PxVehicleLimits::eMAX_NB_WHEELS];
			DirectDrivetrainParams				directDrivetrainParamsDesc;
			//VehicleBrakeCommandResponse			handBrakeCommandResponseDesc;
		};















	}

	struct PxParticleBufferDesc
	{
		PxVec4* positions;               // 파티클의 위치와 반질량(1/mass)
		PxVec4* velocities;              // 파티클의 속도
		PxU32* phases;                  // 파티클의 phase 정보
		PxParticleVolume* volumes;       // 파티클의 볼륨 정보 (선택적)
		PxU32   numActiveParticles;      // 활성화된 파티클 수
		PxU32   maxParticles;            // 최대 파티클 수
		PxU32   numVolumes;              // 볼륨 수 (선택적)
		PxU32   maxVolumes;              // 최대 볼륨 수 (선택적)

		PxParticleBufferDesc()           // 초기화 리스트를 통한 기본 생성자
			: positions(NULL),
			velocities(NULL),
			phases(NULL),
			volumes(NULL),
			numActiveParticles(0),
			maxParticles(0),
			numVolumes(0),
			maxVolumes(0)
		{ }
	};


	struct ENGINE_DLL PxParticleAndDiffuseBufferDesc : public PxParticleBufferDesc
	{
		PxDiffuseParticleParams diffuseParams;
		PxU32 maxDiffuseParticles;
		PxU32 maxActiveDiffuseParticles;
		PxParticleAndDiffuseBufferDesc() : PxParticleBufferDesc() { }
	};

	
}
#endif // Engine_Struct_h__
