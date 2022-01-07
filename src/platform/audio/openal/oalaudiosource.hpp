#include "openal.hpp"

#include "platform/audio/audiosource.hpp"

#include <map>

namespace xengine {
    class OALAudioSource : public AudioSource {
    public:
        explicit OALAudioSource(ALuint sourceHandle);

        ~OALAudioSource() override;

        void play() override;

        void pause() override;

        void stop() override;

        void rewind() override;

        void setPitch(float pitch) override;

        float getPitch() override;

        void setGain(float gain) override;

        float getGain() override;

        void setMaxDistance(float maxDistance) override;

        float getMaxDistance() override;

        void setRollOffFactor(float rollOffFactor) override;

        float getRollOffFactor() override;

        void setReferenceDistance(float referenceDistance) override;

        float getReferenceDistance() override;

        void setMininumGain(float minGain) override;

        float getMinimumGain() override;

        void setMaximumGain(float maxGain) override;

        float getMaximumGain() override;

        void setConeOuterGain(float coneOuterGain) override;

        float getConeOuterGain() override;

        void setConeInnerAngle(float innerAngle) override;

        float getConeInnerAngle() override;

        void setConeOuterAngle(float outerAngle) override;

        float getConeOuterAngle() override;

        void setPosition(Vec3f position) override;

        Vec3f getPosition() override;

        void setVelocity(Vec3f velocity) override;

        Vec3f getVelocity() override;

        void setDirection(Vec3f direction) override;

        Vec3f getDirection() override;

        void setSourceRelative(bool relative) override;

        bool getSourceRelative() override;

        void setSourceType(SourceType type) override;

        SourceType getSourceType() override;

        void setLooping(bool looping) override;

        bool getLooping() override;

        SourceState getState() override;

        void setBuffer(const AudioBuffer &buffer) override;

        void clearBuffer() override;

        void queueBuffers(std::vector<std::reference_wrapper<const AudioBuffer>> buffers) override;

        std::vector<std::reference_wrapper<const AudioBuffer>> unqueueBuffers() override;

        ALuint getHandle();

    private:
        ALuint handle;

        std::map<ALuint, std::reference_wrapper<const AudioBuffer>> bufferMapping; //Mapping for returning unqueued buffers.
    };
}