TEMPLATE = lib

TARGET = spine

CONFIG -= lib_bundle
CONFIG -= qt
CONFIG += staticlib

PROJ = $$PWD/../..
include($$PROJ/app.pri)

CONFIG += no_keywords #since "slots" is used in spine-c source code. we have to use no_keywords to fix the compiling error

INCLUDEPATH += include/

# Input
SOURCES += \
    src/spine/Animation.c \
    src/spine/AnimationState.c \
    src/spine/AnimationStateData.c \
    src/spine/Array.c \
    src/spine/Atlas.c \
    src/spine/AtlasAttachmentLoader.c \
    src/spine/Attachment.c \
    src/spine/AttachmentLoader.c \
    src/spine/Bone.c \
    src/spine/BoneData.c \
    src/spine/BoundingBoxAttachment.c \
    src/spine/ClippingAttachment.c \
    src/spine/Color.c \
    src/spine/Event.c \
    src/spine/EventData.c \
    src/spine/extension.c \
    src/spine/Json.c \
    src/spine/MeshAttachment.c \
    src/spine/PathAttachment.c \
    src/spine/PathConstraint.c \
    src/spine/PathConstraintData.c \
    src/spine/PointAttachment.c \
    src/spine/RegionAttachment.c \
    src/spine/Skeleton.c \
    src/spine/SkeletonBinary.c \
    src/spine/SkeletonBounds.c \
    src/spine/SkeletonClipping.c \
    src/spine/SkeletonData.c \
    src/spine/SkeletonJson.c \
    src/spine/Skin.c \
    src/spine/Slot.c \
    src/spine/SlotData.c \
    src/spine/IkConstraint.c \
    src/spine/IkConstraintData.c \
    src/spine/TransformConstraint.c \
    src/spine/TransformConstraintData.c \
    src/spine/Triangulator.c \
    src/spine/VertexAttachment.c \
    src/spine/VertexEffect.c 

HEADERS += \
    src/spine/Json.h \
    include/spine/Animation.h \
    include/spine/AnimationState.h \
    include/spine/AnimationStateData.h \
    include/spine/Array.h \
    include/spine/Atlas.h \
    include/spine/AtlasAttachmentLoader.h \
    include/spine/Attachment.h \
    include/spine/AttachmentLoader.h \
    include/spine/Bone.h \
    include/spine/BoneData.h \
    include/spine/BoundingBoxAttachment.h \
    include/spine/ClippingAttachment.h \
    include/spine/Color.h \
    include/spine/dll.h \
    include/spine/Event.h \
    include/spine/EventData.h \
    include/spine/extension.h \
    include/spine/MeshAttachment.h \
    include/spine/PathAttachment.h \
    include/spine/PathConstraint.h \
    include/spine/PathConstraintData.h \
    include/spine/PointAttachment.h \
    include/spine/RegionAttachment.h \
    include/spine/Skeleton.h \
    include/spine/SkeletonBinary.h \
    include/spine/SkeletonBounds.h \
    include/spine/SkeletonClipping.h \
    include/spine/SkeletonData.h \
    include/spine/SkeletonJson.h \
    include/spine/Skin.h \
    include/spine/SkinnedMeshAttachment.h \
    include/spine/Slot.h \
    include/spine/SlotData.h \
    include/spine/spine.h \
    include/spine/IkConstraint.h \
    include/spine/IkConstraintData.h \
    include/spine/TransformConstraint.h \
    include/spine/TransformConstraintData.h \
    include/spine/Triangulator.h \
    include/spine/VertexAttachment.h \
    include/spine/VertexEffect.h



