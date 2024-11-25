#pragma once

#include "../Enums/eEventType.h"

class CPedGroup;
struct CEntityGTA;
enum eEventType;
struct CPedGTA;

class CEvent {
public:
    int32 m_nTimeActive;
    bool  m_bValid;
    bool  field_9;
    bool  field_A;
    bool  field_B;

public:
    static void* operator new(size_t size) {
        return ((CEvent * (__cdecl*)(uint32))(g_libGTASA + (VER_x32 ? 0x0036FB60 + 1 : 0x441324)))(size);
    }

    static void operator delete(void* object) {
        ((void(__cdecl*)(void*))(g_libGTASA + (VER_x32 ? 0x0036FBC4 + 1 : 0x4413A8)))(object);
    }

    CEvent();

    virtual ~CEvent();
    virtual eEventType GetEventType() const = 0;
    virtual int32 GetEventPriority() const = 0;
    virtual int32 GetLifeTime() = 0;
    virtual CEvent* Clone() = 0;
    virtual bool AffectsPed(CPedGTA* ped) { return true; };
    virtual bool AffectsPedGroup(CPedGroup* pedGroup) { return true; };
    virtual bool IsCriminalEvent() { return false; }
    virtual void ReportCriminalEvent(CPedGTA* ped) { }; // empty
    virtual bool HasEditableResponse() const { return false; }
    virtual CEntityGTA* GetSourceEntity() const { return nullptr; }
    virtual bool TakesPriorityOver(const CEvent& refEvent) { return GetEventPriority() >= refEvent.GetEventPriority(); }
    virtual float GetLocalSoundLevel() { return 0.0f; }
    virtual bool DoInformVehicleOccupants(CPedGTA* ped) { return false; }
    virtual bool IsValid(CPedGTA* ped) { return m_bValid || m_nTimeActive <= GetLifeTime(); }
    virtual bool CanBeInterruptedBySameEvent() { return false; };

    float GetSoundLevel(const CEntityGTA* entity, CVector& position);
    static float CalcSoundLevelIncrement(float level1, float level2);

    /// Works like `dynamic_cast` => Checks if the event if ofthe required type, if so, returns it, otherwise nullptr
    template <typename T>
    static T* DynCast(CEvent* event) {
        if (event) {
            if (event->GetEventType() == T::Type) {
                return static_cast<T*>(event);
            }
        }
        return nullptr;
    }
};
VALIDATE_SIZE(CEvent, (VER_x32 ? 0xC : 0x10));
