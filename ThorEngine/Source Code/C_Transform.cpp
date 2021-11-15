#include "C_Transform.h"
#include "Component.h"
#include "GameObject.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/matrix_major_storage.hpp"

#include "glm/glm.hpp" // TODO: We include EVERYTHING just to get hints, we should include only the specified files

glm::mat4x4 FromMathGeoLib(const float4x4& originalMat)
{
	glm::vec4 col0 = glm::vec4(originalMat[0][0], originalMat[1][0], originalMat[2][0], originalMat[3][0]);
	glm::vec4 col1 = glm::vec4(originalMat[0][1], originalMat[1][1], originalMat[2][1], originalMat[3][1]);
	glm::vec4 col2 = glm::vec4(originalMat[0][2], originalMat[1][2], originalMat[2][2], originalMat[3][2]);
	glm::vec4 col3 = glm::vec4(originalMat[0][3], originalMat[1][3], originalMat[2][3], originalMat[3][3]);
	return glm::mat4x4(col0, col1, col2, col3);
}

float4x4 FromMathGL(const glm::mat4x4& originalMat)
{
	float4 col0 = float4(originalMat[0][0], originalMat[1][0], originalMat[2][0], originalMat[3][0]);
	float4 col1 = float4(originalMat[0][1], originalMat[1][1], originalMat[2][1], originalMat[3][1]);
	float4 col2 = float4(originalMat[0][2], originalMat[1][2], originalMat[2][2], originalMat[3][2]);
	float4 col3 = float4(originalMat[0][3], originalMat[1][3], originalMat[2][3], originalMat[3][3]);
	return float4x4(col0, col1, col2, col3);
}

C_Transform::C_Transform(GameObject* parent, float3 position, Quat rotation, float3 scale) : Component(Component::Type::Transform, parent, false),
						localPosition(position), localRotation(rotation), localScale(scale),
						localPosition_glm(position.x, position.y, position.z),
						localRotation_glm(rotation.w, rotation.x, rotation.y, rotation.z),
						localScale_glm(scale.x, scale.y, scale.z)
{
	SetLocalTransform(localPosition_glm, localRotation_glm, localScale_glm);
}

C_Transform::C_Transform(GameObject* parent, const float4x4& transform) : Component(Component::Type::Transform, parent, false)
{
	SetLocalTransform(FromMathGeoLib(transform));
}

C_Transform::~C_Transform()
{

}

void C_Transform::SetLocalTransform(glm::mat4x4 newTransform) // private
{
	// NOTE: Purely a test to see how them matrices behave
	glm::mat4x4 matrixTranslation_glm = glm::translate(glm::mat4x4(1.0f), glm::vec3(5.0f, 2.0f, 3.0f));
	float4x4 matrixTranslation = float4x4::Translate(float3(5.0f, 2.0f, 3.0f));

	localTransform_glm = newTransform;

	glm::vec3 skewVector;
	glm::vec4 perspectiveVector;
	glm::decompose(localTransform_glm, localScale_glm, localRotation_glm, localPosition_glm, skewVector, perspectiveVector);

	localRotationEuler_glm = glm::eulerAngles(localRotation_glm) * RADTODEG;

	// TODO: Erase later
	localTransform = FromMathGL(newTransform);
	localTransform.Decompose(localPosition, localRotation, localScale);
	localRotationEuler = localRotation.ToEulerXYZ() * RADTODEG;

	UpdateTransformHierarchy();
}

void C_Transform::SetLocalTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) // private
{
	glm::mat4x4 translationMatrix = glm::translate(glm::mat4x4(1.0f), position);
	glm::mat4x4 rotationMatrix = (glm::mat4x4)rotation;
	glm::mat4x4 scaleMatrix = glm::scale(glm::mat4x4(1.0f), scale);

	glm::mat4x4 newLocalTransform = translationMatrix * rotationMatrix * scaleMatrix;
	SetLocalTransform(newLocalTransform);
}

void C_Transform::SetLocalEulerRotation(float3 euler_angles)
{
	// TODO: Erase later
	float3 delta = (euler_angles - localRotationEuler) * DEGTORAD;
	Quat newRotation = localRotation * Quat::FromEulerXYZ(delta.x, delta.y, delta.z);

	// TODO: Can we directly set the new quaternion with the new GLMath change?
	glm::vec3 delta_glm = (glm::vec3(euler_angles.x, euler_angles.y, euler_angles.z) - localRotationEuler_glm) * DEGTORAD;
	glm::quat newRotation_glm = localRotation_glm * glm::quat(delta_glm);

	SetLocalTransform(localPosition_glm, newRotation_glm, localScale_glm);
}

void C_Transform::SetGlobalTransform(float4x4 transform)
{
	localTransform_glm = FromMathGeoLib(transform);
	if (gameObject->parent)
	{
		float4x4 parentTransposed = gameObject->parent->GetComponent<C_Transform>()->GetTransform().Transposed();
		localTransform_glm = FromMathGeoLib(parentTransposed) * FromMathGeoLib(transform);
	}
	SetLocalTransform(localTransform_glm);

	// TODO: Erase later
	// Recalculate local transform depending on the parent transform
	localTransform = transform;
	if (gameObject->parent)
	{
		//TODO: This should, theoretically, be changed to Inversed instead of Transposed. Review matrix equation solving
		localTransform = gameObject->parent->GetComponent<C_Transform>()->GetTransform().Transposed() * transform; 
	}
	//SetLocalTransform(localTransform);
}

void C_Transform::SetPosition(float3 newPosition)
{
	transform_glm[3] = glm::vec4(newPosition.x, newPosition.y, newPosition.z, transform_glm[3][3]);
	
	// TODO: Erase later
	transform.SetTranslatePart(newPosition);

	SetGlobalTransform(FromMathGL(transform_glm));
}

void C_Transform::SetRotationAxis(float3 x, float3 y, float3 z)
{
	// NOTE: Continue here. Receive MathGeoLib parameters and convert them to OpenGL

	transform.SetCol3(0, x);	
	transform.SetCol3(1, y);
	transform.SetCol3(2, z);

	SetGlobalTransform(transform);
}

void C_Transform::LookAt(float3 target, float3 worldUp)
{
	float3 dir = target - GetPosition();

	float3 fwd = dir.Normalized();
	float3 right = worldUp.Cross(fwd).Normalized();
	float3 up = fwd.Cross(right).Normalized();

	SetRotationAxis(right, up, fwd);
}

void C_Transform::Reset()
{
	SetLocalTransform(float4x4::identity);
}

void C_Transform::OnTransformUpdated()
{
	transform_updated = false;
}

void C_Transform::Serialize(Config& config)
{
	Component::Serialize(config);
	if (config.isSaving)
	{
		config.SetArray("Translation").AddFloat3(localPosition);
		config.SetArray("Rotation").AddQuat(localRotation);
		config.SetArray("Scale").AddFloat3(localScale);
	}
	else
	{
		localPosition = config.GetArray("Translation").GetFloat3(0, float3::zero);
		localRotation = config.GetArray("Rotation").GetQuat(0, Quat::identity);
		localScale = config.GetArray("Scale").GetFloat3(0, float3::one);

		localTransform = float4x4::FromTRS(localPosition, localRotation, localScale);
		transform_updated = true;

		localRotationEuler = localRotation.ToEulerXYZ() * RADTODEG;
	}
}

void C_Transform::UpdateTransformHierarchy()
{
	transform = gameObject->parent ? gameObject->parent->GetComponent<C_Transform>()->GetTransform() * localTransform : localTransform;
	transformT = transform.Transposed();
	transform_updated = true;

	// Recursively update child transforms
	for (uint i = 0; i < gameObject->childs.size(); ++i)
	{
		gameObject->childs[i]->GetComponent<C_Transform>()->UpdateTransformHierarchy();
	}
}