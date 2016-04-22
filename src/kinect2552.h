

//bugbug fix path in debug and release mode
#include <C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc\Kinect.Face.h>

//https://github.com/Vangos, http://www.cs.princeton.edu/~edwardz/tutorials/kinect2/kinect0_sdl.html, https://github.com/UnaNancyOwen
//http://www.pointclouds.org/
//https://books.google.com/books?id=CVUpCgAAQBAJ&pg=PA167&lpg=PA167&dq=GetAudioBodyCorrelation&source=bl&ots=UENPsaMG_J&sig=5RHwdiXn4T7gst6lVt9SFvp2ahw&hl=en&sa=X&ved=0ahUKEwjK9umrqL7KAhUIVz4KHVL-BEYQ6AEIPTAG#v=onepage&q=GetAudioBodyCorrelation&f=false
// https://github.com/Microsoft/DirectX-Graphics-Samples

namespace Software2552 {
	class KinectBaseClass {
	public:
	};

	// can be, but does not need to be, a base class as its all static and can just be called, could not even be a class I suppose
	class Trace : public KinectBaseClass {
	public:
		static bool checkPointer2(IUnknown *p, const string&  message, char*file = __FILE__, int line = __LINE__);
		static bool checkPointer2(KinectBaseClass *p, const string&  message, char*file = __FILE__, int line = __LINE__);

		static void logError2(const string& error, char*file, int line);
		static void logVerbose2(const string& message, char*file, int line) {
			if (ofGetLogLevel() >= OF_LOG_VERBOSE) {
				ofLog(OF_LOG_VERBOSE, buildString(message, file, line));
			}
		}
		static void logTraceBasic(const string& message, char *name);
		static void logTraceBasic(const string& message);
		static void logTrace2(const string& message, char*file, int line);
		static void logError2(HRESULT hResult, const string&  message, char*file, int line);
		static bool CheckHresult2(HRESULT hResult, const string& message, char*file, int line);
		static string buildString(const string& errorIn, char* file, int line);
		static std::string wstrtostr(const std::wstring &wstr);

		// get the right line number bugbug add DEBUG ONLY
#define logError(p1, p2) Trace::logError2(p1, p2, __FILE__, __LINE__)
#define logErrorString(p1) Trace::logError2(p1, __FILE__, __LINE__)
#if _DEBUG
#define logVerbose(p1) Trace::logVerbose2(p1, __FILE__, __LINE__)
#else
#define logVerbose(p1) 
#endif
#define logTrace(p1) Trace::logTrace2(p1, __FILE__, __LINE__)
#define checkPointer(p1, p2) Trace::checkPointer2(p1, p2, __FILE__, __LINE__)
#define checkPointer(p1, p2) Trace::checkPointer2(p1, p2, __FILE__, __LINE__)
#define hresultFails(p1, p2) Trace::CheckHresult2(p1, p2, __FILE__, __LINE__)
#define basicTrace(p) Trace::logTraceBasic(p, #p)
	};

	template<class Interface> void SafeRelease(Interface *& pInterfaceToRelease)
	{
		if (pInterfaceToRelease != NULL) {
			pInterfaceToRelease->Release();
			pInterfaceToRelease = NULL;
		}
	}

	class KinectAudioStream : public IStream	{
	public:
		KinectAudioStream(IStream *p32BitAudio) : m_cRef(1), m_p32BitAudio(p32BitAudio), m_SpeechActive(false) {}

		void SetSpeechState(bool state) { m_SpeechActive = state; }

		STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
		STDMETHODIMP_(ULONG) Release();
		STDMETHODIMP QueryInterface(REFIID riid, void **ppv);

	private:
		STDMETHODIMP Read(void *, ULONG, ULONG *);
		STDMETHODIMP Write(const void *, ULONG, ULONG *) { return E_NOTIMPL; }
		STDMETHODIMP Seek(LARGE_INTEGER, DWORD, ULARGE_INTEGER *) { return S_OK; }
		STDMETHODIMP SetSize(ULARGE_INTEGER) { return E_NOTIMPL; }
		STDMETHODIMP CopyTo(IStream *, ULARGE_INTEGER, ULARGE_INTEGER *, ULARGE_INTEGER *) { return E_NOTIMPL; }
		STDMETHODIMP Commit(DWORD) { return E_NOTIMPL; }
		STDMETHODIMP Revert() { return E_NOTIMPL; }
		STDMETHODIMP LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }
		STDMETHODIMP UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return E_NOTIMPL; }
		STDMETHODIMP Stat(STATSTG *, DWORD) { return E_NOTIMPL; }
		STDMETHODIMP Clone(IStream **) { return E_NOTIMPL; }
		UINT                    m_cRef;
		IStream*                m_p32BitAudio;
		bool                    m_SpeechActive;
	};

	
	class Kinect2552 : public KinectBaseClass {
	public:
		Kinect2552() :KinectBaseClass() {	}

		~Kinect2552();

		bool setup(WriteComms &comms);

		IKinectSensor* getSensor() {
			return pSensor;
		}
		IBodyFrameReader* getBodyReader() {
			return pBodyReader;
		}
		IBodyIndexFrameReader* getBodyIndexReader() {
			return pBodyIndexReader;
		}
		static const int personCount = BODY_COUNT;
		HRESULT depth(UINT cameraPointCount, CameraSpacePoint*csp, UINT depthPointCount, DepthSpacePoint *dsp) { return pCoordinateMapper->MapCameraPointsToDepthSpace(1, csp, 1, dsp); }
		HRESULT color(UINT cameraPointCount, const CameraSpacePoint*csp, UINT depthPointCount, ColorSpacePoint *color) { return pCoordinateMapper->MapCameraPointsToColorSpace(1, csp, 1, color); }
	private:

		IKinectSensor*     pSensor = nullptr;

		IColorFrameReader* pColorReader = nullptr;
		IColorFrameSource* pColorSource = nullptr;

		IDepthFrameReader* pDepthReader = nullptr; // depth and IR not deeply support, bugbug should they be? not sure.
		IDepthFrameSource* pDepthSource = nullptr;

		IFrameDescription* pDescriptionColor = nullptr;
		IFrameDescription* pDescriptionDepth = nullptr;

		IBodyFrameReader*  pBodyReader = nullptr;
		IBodyFrameSource*  pBodySource = nullptr;

		IBodyIndexFrameSource* pBodyIndexSource = nullptr;
		IBodyIndexFrameReader* pBodyIndexReader = nullptr;

		ICoordinateMapper* pCoordinateMapper = nullptr;
	};


	//base class for things like faces
	class BodyItems : public KinectBaseClass {
	public:
		BodyItems() :KinectBaseClass(){}
		void setupKinect(Kinect2552 *pKinectIn) {pKinect = pKinectIn;	};
		Kinect2552 *getKinect() { checkPointer(pKinect, "getKinect"); return pKinect; }

	protected:
		virtual void setTrackingID(int index, UINT64 trackingId) {};

	private:
		Kinect2552 *pKinect;
	};

	class KinectBody : public BodyItems {
	public:
		KinectBody::KinectBody(Kinect2552 *pKinect) {setupKinect(pKinect);}

		Joint joints[JointType::JointType_Count];
		HandState leftHandState = HandState::HandState_Unknown;
		HandState rightHandState = HandState::HandState_Unknown;
		PointF leanAmount;
		
	};

	
	//bugbug remove
	class KinectFace : public BodyItems {
	public:
		KinectFace(Kinect2552 *pKinect = nullptr) {
			setupKinect(pKinect);
		}
		void cleanup();

		IFaceFrameReader* getFaceReader() {
			checkPointer(pFaceReader, "getFaceReader");
			return pFaceReader;
		}
		IFaceFrameSource* getFaceSource() {
			checkPointer(pFaceSource, "getFaceSource");
			return pFaceSource;
		}

		friend class KinectFaces;
		
	protected:
		PointF facePoint[FacePointType::FacePointType_Count];
		PointF facePointIR[FacePointType::FacePointType_Count];
		DetectionResult faceProperty[FaceProperty::FaceProperty_Count];
		RectI boundingBox;
		Vector4 faceRotation;
		IFaceFrameReader* pFaceReader;
		IFaceFrameSource* pFaceSource;

	};

	class KinectFaces : public BodyItems {
	public:
		~KinectFaces();

		void setup(Kinect2552 *);
		void update(WriteComms &comms);
	protected:
		vector<shared_ptr<KinectFace>> faces;
		void setTrackingID(int index, UINT64 trackingId);
		
		// features are the same for all faces
		DWORD features = FaceFrameFeatures::FaceFrameFeatures_BoundingBoxInColorSpace
			| FaceFrameFeatures::FaceFrameFeatures_PointsInColorSpace
			| FaceFrameFeatures::FaceFrameFeatures_RotationOrientation
			| FaceFrameFeatures::FaceFrameFeatures_Happy
			| FaceFrameFeatures::FaceFrameFeatures_RightEyeClosed
			| FaceFrameFeatures::FaceFrameFeatures_LeftEyeClosed
			| FaceFrameFeatures::FaceFrameFeatures_MouthOpen
			| FaceFrameFeatures::FaceFrameFeatures_MouthMoved
			| FaceFrameFeatures::FaceFrameFeatures_LookingAway
			| FaceFrameFeatures::FaceFrameFeatures_Glasses
			| FaceFrameFeatures::FaceFrameFeatures_FaceEngagement;
	private:
		void buildFaces();

	};

	class KinectAudio : public BodyItems {
	public:
		friend class KinectBodies;

		KinectAudio(Kinect2552 *pKinect = nullptr);
		~KinectAudio();

		void setup(Kinect2552 *pKinect);

		void getAudioCorrelation(WriteComms &comms);
		UINT64 getTrackingID() { return audioTrackingId; }

	protected:
		bool confident() { return  getConfidence() > 0.5f; }
		float getAngle() { return angle; }
		float getConfidence() { return confidence; }
		void getAudioBeam(WriteComms &comms);
		void getAudioCommands(WriteComms &comms);
		int  getTrackingBodyIndex() { return trackingIndex; }
		virtual void setTrackingID(int index, UINT64 trackingId);
		HRESULT createSpeechRecognizer();
		HRESULT startSpeechRecognition();

	private:
		void update(WriteComms &comms);
		const UINT64 NoTrackingID = _UI64_MAX - 1;
		const UINT64 NoTrackingIndex = -1;
		IAudioBeamFrameReader* getAudioBeamReader() {	return pAudioBeamReader;	}
		IAudioSource* getAudioSource() {		return pAudioSource;	}

		HRESULT findKinect();
		HRESULT setupSpeachStream();

		IAudioSource*		   pAudioSource = nullptr;
		IAudioBeamFrameReader* pAudioBeamReader = nullptr;
		IAudioBeamList* pAudioBeamList = nullptr;
		IAudioBeam* pAudioBeam = nullptr;
		IStream* pAudioStream = nullptr;
		ISpStream* pSpeechStream = nullptr;
		ISpRecognizer* pSpeechRecognizer = nullptr;
		ISpRecoContext* pSpeechContext = nullptr;
		ISpRecoGrammar* pSpeechGrammar = nullptr;
		HANDLE hSpeechEvent = INVALID_HANDLE_VALUE;
		KinectAudioStream* audioStream = nullptr;
		UINT64 audioTrackingId=0;
		int trackingIndex=0;
		float angle = 0.0f;
		float confidence = 0.0f;
		UINT32 correlationCount = 0;
	};

	class KinectBodies : public KinectFaces {
	public:
		KinectBodies() : KinectFaces() { useFaces(); useAudio(); }
		void update(WriteComms &comms);
		void setup(Kinect2552 *kinectInput);

		void useFaces(bool usefaces = true)  { includeFaces = usefaces; }
		bool usingFaces() { return includeFaces; }

		void useAudio(bool useaudio = true) { includeAudio = useaudio; }
		bool usingAudio() { return includeAudio; }

	private:
		bool includeFaces;
		bool includeAudio;
		void setTrackingID(int index, UINT64 trackingId);
		KinectAudio audio;
	};
}
