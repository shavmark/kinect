#include "ofApp.h"

//SetKinectTwoPersonSystemEngagement vs one?
//InputPointerManager.TransformInputPointerCoordinatesToWindowCoordinates
// https://social.msdn.microsoft.com/Forums/en-US/f29a202a-fa27-4cfc-9079-5addad0906e0/how-can-i-map-a-depth-frame-to-camera-space-without-having-a-kinect-on-hand?forum=kinectv2sdk
//file:///C:/Users/mark/Downloads/KinectHIG.2.0.pdf

namespace Software2552 {
IBodyFrame* getBody(IMultiSourceFrame* frame) {
		IBodyFrame* bodyframe=nullptr;
		if (frame) {
			IBodyFrameReference* pFrameref = NULL;
			frame->get_BodyFrameReference(&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&bodyframe);
				pFrameref->Release();
			}
		}
		return bodyframe;
	}
	IBodyIndexFrame* getBodyIndex(IMultiSourceFrame* frame) {
		IBodyIndexFrame* bodyIndexframe = nullptr;
		if (frame) {
			IBodyIndexFrameReference* pFrameref = NULL;
			frame->get_BodyIndexFrameReference(&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&bodyIndexframe);
				pFrameref->Release();
			}
		}
		return bodyIndexframe;
	}
	IInfraredFrame* getInfrared(IMultiSourceFrame* frame) {
		IInfraredFrame* infraredframe = nullptr;
		if (frame) {
			IInfraredFrameReference* pFrameref = NULL;
			frame->get_InfraredFrameReference(&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&infraredframe);
				pFrameref->Release();
			}
		}
		return infraredframe;
	}
	ILongExposureInfraredFrame* getLongExposureInfrared(IMultiSourceFrame* frame) {
		ILongExposureInfraredFrame* infraredframe = nullptr;
		if (frame) {
			
			ILongExposureInfraredFrameReference* pFrameref = NULL;
			frame->get_LongExposureInfraredFrameReference (&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&infraredframe);
				pFrameref->Release();
			}
		}
		return infraredframe;
	}
	IDepthFrame* getDepth(IMultiSourceFrame* frame) {
		IDepthFrame* depthframe = nullptr;
		if (frame) {
			IDepthFrameReference* pFrameref = NULL;
			frame->get_DepthFrameReference(&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&depthframe);
				pFrameref->Release();
			}
		}
		return depthframe;
	}
	IColorFrame* getColor(IMultiSourceFrame* frame) {
		IColorFrame* colorframe=nullptr;
		if (frame) {
			IColorFrameReference* pFrameref = NULL;
			frame->get_ColorFrameReference(&pFrameref);
			if (pFrameref) {
				pFrameref->AcquireFrame(&colorframe);
				pFrameref->Release();
			}
		}
		return colorframe;
	}

	Kinect2552::~Kinect2552() {
		if (pSensor) {
			pSensor->Close();
		}
		SafeRelease(pSensor);
		SafeRelease(pCoordinateMapper);
	}
	//const int width = 512;
	//const const int height = 424;
	//const const int colorwidth = 1920;
	//const const int colorheight = 1080;
	void depth2RGB(Kinect2552 *kinect, unsigned short*buffer, float*destrgb, unsigned char*rgbimage) {
		ColorSpacePoint * depth2rgb = new ColorSpacePoint[512 * 424];     // Maps depth pixels to rgb pixels
		if (depth2rgb) {
			HRESULT hResult = kinect->getMapper()->MapDepthFrameToColorSpace(
				512 * 424, buffer,        // Depth frame data and size of depth frame
				512 * 424, depth2rgb); // Output ColorSpacePoint array and size
									   // Write color array for vertices
			float* fdest = (float*)destrgb;
			for (int i = 0; i < 512 * 424; i++) {
				ColorSpacePoint p = depth2rgb[i];
				// Check if color pixel coordinates are in bounds
				if (p.X < 0 || p.Y < 0 || p.X > 1920 || p.Y > 1080) {
					*fdest++ = 0;
					*fdest++ = 0;
					*fdest++ = 0;
				}
				else {
					int idx = (int)p.X + 1920 * (int)p.Y;
					*fdest++ = rgbimage[4 * idx + 0] / 255.;
					*fdest++ = rgbimage[4 * idx + 1] / 255.;
					*fdest++ = rgbimage[4 * idx + 2] / 255.;
				}
				// Don't copy alpha channel
			}
			delete depth2rgb;
		}
	}
	void depth2XYZ(Kinect2552 *kinect, unsigned short*buffer, float*destXYZ) {
		CameraSpacePoint * depth2xyz = new CameraSpacePoint[512 * 424];     // Maps depth pixels to rgb pixels
		if (depth2xyz) {
			HRESULT hResult = kinect->getMapper()->MapDepthFrameToCameraSpace(
				512 * 424, buffer,        // Depth frame data and size of depth frame
				512 * 424, depth2xyz); // Output CameraSpacePoint array and size
			for (int i = 0; i <512 * 424; i++) { // map to points (not sure what this is yet)
				destXYZ[3 * i + 0] = depth2xyz[i].X;
				destXYZ[3 * i + 1] = depth2xyz[i].Y;
				destXYZ[3 * i + 2] = depth2xyz[i].Z;
			}
			delete depth2xyz;
		}

	}
	
	void KinectBody::updateImage(ofImage& image, IMultiSourceFrame* frame) {
		IBodyIndexFrame * bodyindex = getBodyIndex(frame);
		if (!bodyindex) {
			return;
		}
		int width = 512;
		int height = 424;
		unsigned int bufferSize = 0;
		unsigned char* buffer = nullptr;
		unsigned char* output;
		HRESULT hResult = bodyindex->AccessUnderlyingBuffer(&bufferSize, &buffer);
		string s1 = "hello";
		string s2;
		string s3;
		const char*bff =(const char*)buffer;
		size_t t  = snappy::Compress(bff, bufferSize, &s2);
		ofBuffer bufferobject;
		bool b = snappy::Uncompress(s2.c_str(), s2.size(), &s3);
		
		unsigned char*b2 = (unsigned char*)s3.c_str();
		if (SUCCEEDED(hResult)) {
			image.allocate(width, height, OF_IMAGE_COLOR);
			bool found = false;
			for (float y = 0; y < height; y++) {
				for (float x = 0; x < width; x++) {
					unsigned int index = y * width + x;
					if (b2[index] != 0xff) {
						float hue = x / width * 255;
						float sat = ofMap(y, 0, height / 2, 0, 255, true);
						float bri = ofMap(y, height / 2, height, 255, 0, true);
						// make a dynamic image, also there can be up to 6 images so we need them to be a little different 
						image.setColor(x, y, ofColor::fromHsb(hue, sat, bri));
						found = true;
					}
					else {
						image.setColor(x, y, ofColor::white);
					}
				}
			}
			if (!found) {
				image.clear();
			}
			image.update();
		}

		SafeRelease(bodyindex);

	}
	void KinectBody::updateImageIR(ofImage& image, IMultiSourceFrame* frame) {
		IInfraredFrame * ir = getInfrared(frame);
		if (!ir) {
			return;
		}
		int width = 512;
		int height = 424;
		UINT bufferSize = 0;
		UINT16 * buffer = nullptr;
		HRESULT hResult = ir->AccessUnderlyingBuffer(&bufferSize, &buffer);
		int i;
		if (SUCCEEDED(hResult)) {
			image.allocate(width, height, OF_IMAGE_COLOR);
			for (float y = 0; y < height; y++) {
				for (float x = 0; x < width; x++) {
					unsigned int index = y * width + x;
					image.setColor(x, y, ofColor::fromHsb(255, 50, buffer[index]));
				}
			}
			image.update();
		}

		SafeRelease(ir);

	}
	void KinectBody::update(ofImage& image, ofImage& imageir, WriteOsc &comms) {
		IMultiSourceFrame* frame = NULL;
		HRESULT hResult;

		hResult = getKinect()->reader->AcquireLatestFrame(&frame); // gets all data in one shot
		if (hresultFails(hResult, "AcquireLatestFrame")) {
			return;
		}
		IBodyFrame* bodyframe = getBody(frame);
		if (!bodyframe) {
			return;
		}
		updateImage(image, frame);
		updateImageIR(imageir, frame);
		IBody* pBody[BODY_COUNT] = { 0 };

		hResult = bodyframe->GetAndRefreshBodyData(BODY_COUNT, pBody);
		if (!hresultFails(hResult, "GetAndRefreshBodyData")) {
			for (int count = 0; count < BODY_COUNT; count++) {
				// breaks here
				BOOLEAN bTracked = false;

				hResult = pBody[count]->get_IsTracked(&bTracked);
				if (SUCCEEDED(hResult) && bTracked) {
					ofxJSONElement data;
					Joint joints[JointType::JointType_Count];
					HandState leftHandState = HandState::HandState_Unknown;
					HandState rightHandState = HandState::HandState_Unknown;
					PointF leanAmount;

					// Set TrackingID to Detect Face
					UINT64 trackingId = _UI64_MAX;
					hResult = pBody[count]->get_TrackingId(&trackingId);
					if (hresultFails(hResult, "get_TrackingId")) {
						break;
					}
					data["trackingId"] = trackingId;
					if (audio) {
						// see if any audio there
						audio->getAudioCorrelation(comms);
						//bugbug can we use the tracking id, and is valid id, here vs creating our own?
						if (audio->getTrackingID() == trackingId) {
							audio->update(comms);
							data["talking"] = true;
						}
					}
					//setTrackingID(count, trackingId); // use our own tracking id for faces
					if (faces) {
						setTrackingID(count, trackingId);// keep face on track with body
						faces->update(comms, trackingId);//bugbug need to simplfy this but see what happens for now
					}

					// get joints
					hResult = pBody[count]->GetJoints(JointType::JointType_Count, joints);
					if (!hresultFails(hResult, "GetJoints")) {
						// Left Hand State
						hResult = pBody[count]->get_HandLeftState(&leftHandState);
						if (hresultFails(hResult, "get_HandLeftState")) {
							break;
						}
						data["left"] = leftHandState;
						hResult = pBody[count]->get_HandRightState(&rightHandState);
						if (hresultFails(hResult, "get_HandRightState")) {
							break;
						}
						data["right"] = rightHandState;
						// Lean
						hResult = pBody[count]->get_Lean(&leanAmount);
						if (hresultFails(hResult, "get_Lean")) {
							break;
						}
						data["lean"]["x"] = leanAmount.X;
						data["lean"]["y"] = leanAmount.Y;
					}

					comms.send(data, "kinect/body");
					for (int i = 0; i < JointType::JointType_Count; ++i) {
						ofxJSONElement data;
						data["trackingId"] = trackingId;

						CameraSpacePoint position = joints[i].Position;
						DepthSpacePoint depthSpacePoint;
						ColorSpacePoint colorSpacePoint;
						HRESULT hResult = getKinect()->depth(1, &position, 1, &depthSpacePoint);
						if (hresultFails(hResult, "depth")) {
							break;
						}
						hResult = getKinect()->color(1, &position, 1, &colorSpacePoint);
						if (hresultFails(hResult, "color")) {
							break;
						}
						//bugbug maybe track the last one sent and then only send whats changed
						// then the listener just keeps on data set current
						data["jointType"] = joints[i].JointType;
						data["trackingState"] = joints[i].TrackingState;
						data["depth"]["x"] = depthSpacePoint.X;
						data["depth"]["y"] = depthSpacePoint.Y;
						data["color"]["x"] = colorSpacePoint.X;
						data["color"]["y"] = colorSpacePoint.Y;
						data["cam"]["x"] = position.X;
						data["cam"]["y"] = position.Y;
						data["cam"]["z"] = position.Z;
						string s = data.getRawString();
						data["kinectID"] = getKinect()->getId();
						comms.send(data, "kinect/joints");
					}
				}
			}
		}
		for (int count = 0; count < BODY_COUNT; count++) {
			SafeRelease(pBody[count]);
		}
		SafeRelease(bodyframe);
		SafeRelease(frame);
}

bool Kinect2552::setup(WriteOsc &comms) {
		HRESULT hResult = GetDefaultKinectSensor(&pSensor);
		if (hresultFails(hResult, "GetDefaultKinectSensor")) {
			return false;
		}

		hResult = pSensor->Open();
		if (hresultFails(hResult, "IKinectSensor::Open")) {
			return false;
		}
		// we just get body since we are sending data off the box
		// keep color separate https://social.msdn.microsoft.com/Forums/en-US/4672ca22-4ff2-445b-8574-3011ef16a44c/long-exposure-infrared-vs-infrared?forum=kinectv2sdk
		//http://blog.csdn.net/guoming0000/article/details/46392909
		hResult = pSensor->OpenMultiSourceFrameReader(
			 FrameSourceTypes::FrameSourceTypes_Body|
			 FrameSourceTypes_BodyIndex | FrameSourceTypes_Infrared
			,
			&reader);
		if (hresultFails(hResult, "OpenMultiSourceFrameReader")) {
			return false;
		}

		hResult = pSensor->get_CoordinateMapper(&pCoordinateMapper);
		if (hresultFails(hResult, "get_CoordinateMapper")) {
			return false;
		}

		// wait for sensor, will wait for ever
		BOOLEAN avail = false;
		do {
			pSensor->get_IsAvailable(&avail);
			if (avail) {
				break;
			}
			else {
				ofSleepMillis(100);
			}
		} while (1);

		WCHAR id[256] = { 0 };
		hResult = pSensor->get_UniqueKinectId(256, id);
		if (hresultFails(hResult, "get_UniqueKinectId")) {
			return false;
		}
		for (size_t i = 0; i < id[0] != 0; ++i) {
			kinectID += id[i];
		}
		ofxJSONElement data;
		data["width"]["color"] = 1920;
		data["width"]["depth"] = 512;
		data["height"]["color"] = 1080;
		data["height"]["depth"] = 424;
		data["kinectID"] = kinectID;
		comms.send(data, "kinect/install");


		logTrace("Kinect signed on, life is good.");

		return true;
	}

void KinectFace::cleanup()
{
	// do not call in destructor as pointers are used, call when needed
	SafeRelease(pFaceReader);
	SafeRelease(pFaceSource);
}
KinectFaces::~KinectFaces() {

	for (int i = 0; i < faces.size(); ++i) {
		faces[i]->cleanup();
	}

}
void KinectBody::setTrackingID(int index, UINT64 trackingId) {
	if (faces) {
		faces->setTrackingID(index, trackingId);
	}
}
// get the face readers
void KinectFaces::setup() {
	buildFaces();
}

void KinectFaces::setTrackingID(int index, UINT64 trackingId) { 
	if (faces.size() < index) {
		logErrorString("not enough faces");
		return;
	}
	faces[index]->getFaceSource()->put_TrackingId(trackingId);
}

// return true if face found
void KinectFaces::update(WriteOsc &comms, UINT64 trackingId)
{
	for (int count = 0; count < BODY_COUNT; count++) {
		IFaceFrame* pFaceFrame = nullptr;
		HRESULT hResult = faces[count]->getFaceReader()->AcquireLatestFrame(&pFaceFrame); // faces[count].getFaceReader() was pFaceReader[count]
		if (SUCCEEDED(hResult) && pFaceFrame != nullptr) {
			BOOLEAN bFaceTracked = false;
			hResult = pFaceFrame->get_IsTrackingIdValid(&bFaceTracked);
			if (SUCCEEDED(hResult) && bFaceTracked) {
				IFaceFrameResult* pFaceResult = nullptr;
				hResult = pFaceFrame->get_FaceFrameResult(&pFaceResult);
				if (SUCCEEDED(hResult) && pFaceResult != nullptr) {
					UINT64 id;
					pFaceFrame->get_TrackingId(&id);
					if (id != trackingId) {
						return; // not sure abou this yet
					}
					// check for real data first
					Vector4 faceRotation;
					pFaceResult->get_FaceRotationQuaternion(&faceRotation);
					if (!faceRotation.x && !faceRotation.y && !faceRotation.w && !faceRotation.z) {
						return;// noise
					}

					PointF facePoint[FacePointType::FacePointType_Count];
					PointF facePointIR[FacePointType::FacePointType_Count];
					DetectionResult faceProperty[FaceProperty::FaceProperty_Count];
					RectI boundingBox;

					ofxJSONElement data;

					data["trackingId"] = id;

					hResult = pFaceResult->GetFacePointsInColorSpace(FacePointType::FacePointType_Count, facePoint);
					if (hresultFails(hResult, "GetFacePointsInColorSpace")) {
						return;
					}
					data["eye"]["left"]["x"] = facePoint[FacePointType_EyeLeft].X;
					data["eye"]["left"]["y"] = facePoint[FacePointType_EyeLeft].Y;
					data["eye"]["right"]["x"] = facePoint[FacePointType_EyeRight].X;
					data["eye"]["right"]["y"] = facePoint[FacePointType_EyeRight].Y;
					data["nose"]["x"] = facePoint[FacePointType_Nose].X;
					data["nose"]["y"] = facePoint[FacePointType_Nose].Y;
					data["mouth"]["left"]["x"] = facePoint[FacePointType_MouthCornerLeft].X;
					data["mouth"]["left"]["y"] = facePoint[FacePointType_MouthCornerLeft].Y;
					data["mouth"]["right"]["x"] = facePoint[FacePointType_MouthCornerRight].X;
					data["mouth"]["right"]["y"] = facePoint[FacePointType_MouthCornerRight].Y;

					data["faceRotation"]["w"] = faceRotation.w;
					data["faceRotation"]["x"] = faceRotation.x;
					data["faceRotation"]["y"] = faceRotation.y;
					data["faceRotation"]["z"] = faceRotation.z;

					hResult = pFaceResult->GetFaceProperties(FaceProperty::FaceProperty_Count, faceProperty);
					if (hresultFails(hResult, "GetFaceProperties")) {
						return;
					}
#define YES_OR_MAYBE(a)(faceProperty[a] == DetectionResult_Yes || faceProperty[a] == DetectionResult_Maybe)
#define NOT_YES(a)(faceProperty[FaceProperty_RightEyeClosed] != DetectionResult_Yes)
					data["eye"]["right"]["closed"] = NOT_YES(FaceProperty_RightEyeClosed);
					data["eye"]["left"]["closed"] = NOT_YES(FaceProperty_LeftEyeClosed);
					data["mouth"]["open"] = YES_OR_MAYBE(FaceProperty_MouthOpen);
					data["mouth"]["moved"] = YES_OR_MAYBE(FaceFrameFeatures_MouthMoved);
					data["happy"] = YES_OR_MAYBE(FaceProperty_Happy) || faceProperty[FaceProperty_Happy] == DetectionResult_Unknown; // try hard to be happy

					data["lookingAway"] = YES_OR_MAYBE(FaceFrameFeatures_LookingAway);
					data["glasses"] = YES_OR_MAYBE(FaceFrameFeatures_Glasses);
					data["engaged"] = YES_OR_MAYBE(FaceFrameFeatures_FaceEngagement);
					//bugbug if these are relative to current screen convert them to percents of screen (ie x/size kind of thing)
					pFaceResult->get_FaceBoundingBoxInColorSpace(&boundingBox);
					data["boundingBox"]["top"] = boundingBox.Top;
					data["boundingBox"]["left"] = boundingBox.Left;
					data["boundingBox"]["right"] = boundingBox.Right;
					data["boundingBox"]["bottom"] = boundingBox.Bottom;

					data["kinectID"] = getKinect()->getId();
					comms.send(data, "kinect/face");

					SafeRelease(pFaceResult);
					SafeRelease(pFaceFrame);
					return;

				}
				SafeRelease(pFaceResult);
			}
		}
		SafeRelease(pFaceFrame);
	}
}
void KinectFaces::buildFaces() {
	for (int i = 0; i < BODY_COUNT; ++i) {
		shared_ptr<KinectFace> p = make_shared<KinectFace>(getKinect());
		if (p) {
			HRESULT hResult;
			hResult = CreateFaceFrameSource(getKinect()->getSensor(), 0, features, &p->pFaceSource);
			if (hresultFails(hResult, "CreateFaceFrameSource")) {
				// 0x83010001 https://social.msdn.microsoft.com/Forums/en-US/b0d4fb49-5608-49d5-974b-f0044ceac5ca/createfaceframesource-always-returning-error?forum=kinectv2sdk
				return;
			}

			hResult = p->pFaceSource->OpenReader(&p->pFaceReader);
			if (hresultFails(hResult, "face.pFaceSource->OpenReader")) {
				return;
			}

			faces.push_back(p);
		}
	}
}
}