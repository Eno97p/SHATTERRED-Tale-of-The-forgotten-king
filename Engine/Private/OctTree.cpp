#include "OctTree.h"
#include "GameInstance.h"

COctTree::COctTree()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT COctTree::Initialize(const _float3& vMin, const _float3& vMax, _int depth)
{
    m_vMin = vMin;
    m_vMax = vMax;
    m_iLODLevel = depth;

    // m_vCorners 초기화
    m_vCorners[0] = _float3(m_vMin.x, m_vMin.y, m_vMin.z); // 왼쪽 아래 뒤
    m_vCorners[1] = _float3(m_vMax.x, m_vMin.y, m_vMin.z); // 오른쪽 아래 뒤
    m_vCorners[2] = _float3(m_vMax.x, m_vMax.y, m_vMin.z); // 오른쪽 위 뒤
    m_vCorners[3] = _float3(m_vMin.x, m_vMax.y, m_vMin.z); // 왼쪽 위 뒤
    m_vCorners[4] = _float3(m_vMin.x, m_vMin.y, m_vMax.z); // 왼쪽 아래 앞
    m_vCorners[5] = _float3(m_vMax.x, m_vMin.y, m_vMax.z); // 오른쪽 아래 앞
    m_vCorners[6] = _float3(m_vMax.x, m_vMax.y, m_vMax.z); // 오른쪽 위 앞
    m_vCorners[7] = _float3(m_vMin.x, m_vMax.y, m_vMax.z); // 왼쪽 위 앞

    for (int i = 0; i < CORNER_END; ++i)
    {
       // m_iTerrainIndices[i] = terrainIndices[i];
    }

    if (depth >= MAX_DEPTH)
        return S_OK;

    _float3 vSize = { m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y, m_vMax.z - m_vMin.z };
    if (vSize.x <= MIN_NODE_SIZE || vSize.y <= MIN_NODE_SIZE || vSize.z <= MIN_NODE_SIZE)
        return S_OK;

    _float3 vCenter = {
        (m_vMin.x + m_vMax.x) * 0.5f,
        (m_vMin.y + m_vMax.y) * 0.5f,
        (m_vMin.z + m_vMax.z) * 0.5f
    };

    // 자식 노드 생성
    m_pChildren[TOP_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, vCenter.y, vCenter.z), _float3(vCenter.x, m_vMax.y, m_vMax.z), depth + 1);
    m_pChildren[TOP_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, vCenter.y, vCenter.z), _float3(m_vMax.x, m_vMax.y, m_vMax.z), depth + 1);
    m_pChildren[BOTTOM_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, m_vMin.y, vCenter.z), _float3(m_vMax.x, vCenter.y, m_vMax.z), depth + 1);
    m_pChildren[BOTTOM_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, vCenter.z), _float3(vCenter.x, vCenter.y, m_vMax.z), depth + 1);
    m_pChildren[TOP_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, vCenter.y, m_vMin.z), _float3(vCenter.x, m_vMax.y, vCenter.z), depth + 1);
    m_pChildren[TOP_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, vCenter.y, m_vMin.z), _float3(m_vMax.x, m_vMax.y, vCenter.z), depth + 1);
    m_pChildren[BOTTOM_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, m_vMin.y, m_vMin.z), _float3(m_vMax.x, vCenter.y, vCenter.z), depth + 1);
    m_pChildren[BOTTOM_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, m_vMin.z), _float3(vCenter.x, vCenter.y, vCenter.z), depth + 1);

    return S_OK;
}

void COctTree::Culling(const _float4* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
    _float3 vCenter = { (m_vMin.x + m_vMax.x) * 0.5f, (m_vMin.y + m_vMax.y) * 0.5f, (m_vMin.z + m_vMax.z) * 0.5f };
    _float fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vMax) - XMLoadFloat3(&vCenter)));

    if (!m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&vCenter), fRadius))
    {
        return;
    }

    if (nullptr == m_pChildren[TOP_LEFT_FRONT] || isDraw(vCenter))
    {
        // 현재 노드를 그리는 로직
        for (int i = 0; i < CORNER_END; ++i)
        {
            if (m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&m_vCorners[i])))
            {
                // 여기서 적절한 인덱스를 생성하거나 사용
                pIndices[(*pNumIndices)++] = /* 적절한 인덱스 값 */1;
            }
        }
    }
    else
    {
        for (int i = 0; i < CORNER_END; ++i)
        {
            if (m_pChildren[i] != nullptr)
            {
                m_pChildren[i]->Culling(pVertexPositions, pIndices, pNumIndices);
            }
        }
    }
}

_bool COctTree::isDraw(const _float4* pVertexPositions)
{
    _vector vCamPosition = m_pGameInstance->Get_CamPosition();

    _float fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat4(&pVertexPositions[m_iCenterIndex])));

    _float fMaxSize = max(max(m_vMax.x - m_vMin.x, m_vMax.y - m_vMin.y), m_vMax.z - m_vMin.z);

    if (fDistance * 0.1f > fMaxSize)
        return true;

    return false;
}

_bool COctTree::isDraw(const _float3& vCenter)
{
    _vector vCamPosition = m_pGameInstance->Get_CamPosition();
    _float fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat3(&vCenter)));

    _float fLODDistance = m_fMinLODDistance * pow(m_fLODDistanceMultiplier, m_iLODLevel);

    return fDistance > fLODDistance;
}

void COctTree::SetLODParameters(_float minLODDistance, _float lodDistanceMultiplier)
{
    m_fMinLODDistance = minLODDistance;
    m_fLODDistanceMultiplier = lodDistanceMultiplier;
}

void COctTree::Make_Neighbors()
{
    if (m_pChildren[TOP_LEFT_FRONT] == nullptr)
        return;

    for (int i = 0; i < CORNER_END; ++i)
    {
        for (int j = 0; j < NEIGHBOR_END; ++j)
        {
            if (i != j)
            {
                m_pChildren[i]->m_pNeighbors[j] = m_pChildren[j];
            }
        }
    }

    if (m_pNeighbors[NEIGHBOR_LEFT] != nullptr)
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[BOTTOM_RIGHT_FRONT];
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[TOP_RIGHT_BACK];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[BOTTOM_RIGHT_BACK];
    }

    if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
    {
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[BOTTOM_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[BOTTOM_LEFT_BACK];
    }

    if (nullptr != m_pNeighbors[NEIGHBOR_FRONT])
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[TOP_RIGHT_BACK];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[BOTTOM_RIGHT_BACK];
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[BOTTOM_LEFT_BACK];
    }

    if (nullptr != m_pNeighbors[NEIGHBOR_BACK])
    {
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[BOTTOM_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[BOTTOM_LEFT_FRONT];
    }

    if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_RIGHT_FRONT];
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_LEFT_BACK];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_RIGHT_BACK];
    }

    if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
    {
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_RIGHT_BACK];
    }

    for (size_t i = 0; i < CORNER_END; i++)
        m_pChildren[i]->Make_Neighbors();
}

COctTree* COctTree::Create(const _float3& vMin, const _float3& vMax, _int depth)
{
    COctTree* pInstance = new COctTree();

    if (FAILED(pInstance->Initialize(vMin, vMax, depth)))
    {
        MSG_BOX("Failed To Created : COctTree");
        Safe_Release(pInstance);

        return nullptr;
    }

    return pInstance;
}

void COctTree::Free()
{
    for (size_t i = 0; i < CORNER_END; i++)
        Safe_Release(m_pChildren[i]);

    Safe_Release(m_pGameInstance);
}


//void COctTree::Update_OctTree()
//{
//    const _float4 cameraPosition = *m_pGameInstance->Get_CamPosition_float4();
//    m_CamPos = { cameraPosition.x, cameraPosition.y, cameraPosition.z };
//
//    UpdateNodeVisibility();
//}
//
//void COctTree::UpdateNodeVisibility()
//{
//    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
//    {
//        return;
//    }
//
//    for (int i = 0; i < 8; ++i)
//    {
//        m_pChildren[i]->UpdateNodeVisibility();
//    }
//}
//
//void COctTree::AddObject(CGameObject* pObject, PxActor* pActor)
//{
//    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
//    {
//        m_Objects.push_back(pObject);
//        Safe_AddRef(pObject);
//        return;
//    }
//
//    // PhysX Actor에서 바운딩 박스 정보 가져오기
//    physx::PxBounds3 bounds = pActor->getWorldBounds();
//    _float3 objMin = _float3(bounds.minimum.x, bounds.minimum.y, bounds.minimum.z);
//    _float3 objMax = _float3(bounds.maximum.x, bounds.maximum.y, bounds.maximum.z);
//
//    // 바운딩 박스가 현재 노드의 영역을 완전히 포함하는 경우, 현재 노드에 객체 추가
//    if (objMin.x >= m_vMin.x && objMin.y >= m_vMin.y && objMin.z >= m_vMin.z &&
//        objMax.x <= m_vMax.x && objMax.y <= m_vMax.y && objMax.z <= m_vMax.z)
//    {
//        m_Objects.push_back(pObject);
//        Safe_AddRef(pObject);
//        return;
//    }
//
//    // 바운딩 박스와 겹치는 자식 노드에만 객체 추가
//    for (int i = 0; i < 8; ++i)
//    {
//        if (m_pChildren[i]->IsOverlapping(objMin, objMax))
//        {
//            m_pChildren[i]->AddObject(pObject, pActor);
//        }
//    }
//}
//
//bool COctTree::IsOverlapping(const _float3& min, const _float3& max)
//{
//    return (min.x <= m_vMax.x && max.x >= m_vMin.x &&
//        min.y <= m_vMax.y && max.y >= m_vMin.y &&
//        min.z <= m_vMax.z && max.z >= m_vMin.z);
//}
//
//bool COctTree::IsObjectVisible(CGameObject* pObject)
//{
//    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
//    {
//        for (auto* obj : m_Objects)
//        {
//            if (obj == pObject)
//            {
//                return IsNodeVisible();
//            }
//        }
//        return false;
//    }
//
//    for (int i = 0; i < 8; ++i)
//    {
//        if (m_pChildren[i]->IsObjectVisible(pObject))
//        {
//            return true;
//        }
//    }
//
//    return false;
//}
//
//bool COctTree::IsNodeVisible()
//{
//    if (IsNodeOccluded())
//        return false;
//
//    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
//    {
//        return true;
//    }
//
//    for (int i = 0; i < 8; ++i)
//    {
//        if (m_pChildren[i]->IsNodeVisible())
//            return true;
//    }
//
//    return false;
//}
//
//bool COctTree::IsNodeOccluded()
//{
//    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
//    {
//        PxVec3 nodeCenter((m_vMin.x + m_vMax.x) * 0.5f, (m_vMin.y + m_vMax.y) * 0.5f, (m_vMin.z + m_vMax.z) * 0.5f);
//        PxVec3 direction = nodeCenter - m_CamPos;
//        float distance = direction.magnitude();
//        direction.normalize();
//
//        PxRaycastBuffer hit;
//        PxQueryFilterData filterData;
//        filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
//
//        bool isHit = m_pGameInstance->GetScene()->raycast(m_CamPos, direction, distance, hit, PxHitFlag::eDEFAULT, filterData);
//        return isHit;
//    }
//
//    bool isOccluded = true;
//    for (int i = 0; i < 8; ++i)
//    {
//        if (!m_pChildren[i]->IsNodeOccluded())
//        {
//            isOccluded = false;
//            break;
//        }
//    }
//    return isOccluded;
//}

