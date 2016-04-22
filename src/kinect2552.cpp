
#include "ofApp.h"

//SetKinectTwoPersonSystemEngagement vs one?
//InputPointerManager.TransformInputPointerCoordinatesToWindowCoordinates

//file:///C:/Users/mark/Downloads/KinectHIG.2.0.pdf

namespace Software2552 {

	Kinect2552::~Kinect2552() {
		if (pSensor) {
			pSensor->Close();
		}
		SafeRelease(pSensor);
		SafeRelease(pColorReader);
		SafeRelease(pBodyReader);
		SafeRelease(pDepthReader);
		SafeRelease(pDescriptionColor);
		SafeRelease(pDescriptionDepth);
		SafeRelease(pDepthSource);
		SafeRelease(pColorSource);
		SafeRelease(pBodySource);
		SafeRelease(pCoordinateMapper);
		SafeRelease(pBodyIndexSource);
		SafeRelease(pBodyIndexReader);
	}
	void KinectBody::update(WriteComms &comms) {
		IBodyFrame* pBodyFrame = nullptr;
		HRESULT hResult = getKinect()->getBodyReader()->AcquireLatestFrame(&pBodyFrame);
		if (!hresultFails(hResult, "AcquireLatestFrame")) {
			IBody* pBody[Kinect2552::personCount] = { 0 };

			hResult = pBodyFrame->GetAndRefreshBodyData(Kinect2552::personCount, pBody);
			if (!hresultFails(hResult, "GetAndRefreshBodyData")) {
				for (int count = 0; count < Kinect2552::personCount; count++) {
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
							return;
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
								return;
							}
							data["left"] = leftHandState;
							hResult = pBody[count]->get_HandRightState(&rightHandState);
							if (hresultFails(hResult, "get_HandRightState")) {
								return;
							}
							data["right"] = rightHandState;
							// Lean
							hResult = pBody[count]->get_Lean(&leanAmount);
							if (hresultFails(hResult, "get_Lean")) {
								return;
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
			for (int count = 0; count < Kinect2552::personCount; count++) {
				SafeRelease(pBody[count]);
			}
		}

		SafeRelease(pBodyFrame);
}

bool Kinect2552::setup(WriteComms &comms) {
		HRESULT hResult = GetDefaultKinectSensor(&pSensor);
		if (hresultFails(hResult, "GetDefaultKinectSensor")) {
			return false;
		}

		hResult = pSensor->Open();
		if (hresultFails(hResult, "IKinectSensor::Open")) {
			return false;
		}
		
		hResult = pSensor->get_BodyIndexFrameSource(&pBodyIndexSource);
		if (hresultFails(hResult, "get_BodyIndexFrameSource")) {
			return false;
		}

		hResult = pSensor->get_ColorFrameSource(&pColorSource);
		if (hresultFails(hResult, "get_ColorFrameSource")) {
			return false;
		}
		
		hResult = pSensor->get_BodyFrameSource(&pBodySource);
		if (hresultFails(hResult, "get_BodyFrameSource")) {
			return false;
		}

		hResult = pSensor->get_DepthFrameSource(&pDepthSource);
		if (hresultFails(hResult, "get_DepthFrameSource")) {
			return false;
		}

		hResult = pBodyIndexSource->OpenReader(&pBodyIndexReader);
		if (hresultFails(hResult, "pBodyIndexSource OpenReader")) {
			return false;
		}

		hResult = pColorSource->OpenReader(&pColorReader);
		if (hresultFails(hResult, "pColorSource OpenReader")) {
			return false;
		}
		
		hResult = pBodySource->OpenReader(&pBodyReader);
		if (hresultFails(hResult, "pBodySource OpenReader")) {
			return false;
		}

		//bugbug what do we do with these readers?
		hResult = pDepthSource->OpenReader(&pDepthReader);
		if (hresultFails(hResult, "pDepthSource OpenReader")) {
			return false;
		}

		int widthColor = 0; // size of the kinect frames
		int heightColor = 0;
		int widthDepth = 0; // size of the kinect frames
		int heightDepth = 0;

		hResult = pColorSource->get_FrameDescription(&pDescriptionColor);
		if (hresultFails(hResult, "get_FrameDescription pDescriptionColor")) {
			return false;
		}
		pDescriptionColor->get_Width(&widthColor);
		pDescriptionColor->get_Height(&heightColor);

		hResult = pDepthSource->get_FrameDescription(&pDescriptionDepth);
		if (hresultFails(hResult, "get_FrameDescription pDescriptionColor")) {
			return false;
		}
		pDescriptionDepth->get_Width(&widthDepth);
		pDescriptionDepth->get_Height(&heightDepth);

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
		data["width"]["color"] = widthColor;
		data["width"]["depth"] = widthDepth;
		data["height"]["color"] = heightColor;
		data["height"]["depth"] = heightDepth;
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
void KinectFaces::update(WriteComms &comms, UINT64 trackingId)
{
	for (int count = 0; count < Kinect2552::personCount; count++) {
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
	for (int i = 0; i < Kinect2552::personCount; ++i) {
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