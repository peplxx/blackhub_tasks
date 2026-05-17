/*
Задача 2
Дан код простой симуляции системы частиц, поддерживающий приложение импульса и
изменение яркости. Однако, несмотря на кажущуюся простоту, он написана не самым
оптимальным образом. Ваша задача ускорить работу методов класса ParticleSystem.
Для этого вы можете модифицировать весь представленный код: удалять, добавлять и
изменять структуры, модифицировать их методы. Дополнительно в коде помечены
места, которые менять нельзя: несколько структур и сигнатур их методов Задача
считается решенной, если выполняется менее чем за 10 с. на стендовой машине
*/

#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

struct Vec3 {
  // do not remove the default constructor
  Vec3() = default;

  // do not remove the following getters
  float &X() { return x; }
  float &Y() { return y; }
  float &Z() { return z; }
  float X() const { return x; }
  float Y() const { return y; }
  float Z() const { return z; }

  float x;
  float y;
  float z;

  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  Vec3 operator+(const Vec3 &other) const {
    return {x + other.x, y + other.y, z + other.z};
  }

  Vec3 operator*(float scale) const {
    return {x * scale, y * scale, z * scale};
  }

  Vec3 operator/(float scale) const {
    return {x / scale, y / scale, z / scale};
  }
};

// do not change
struct ParticleCInfo {
  Vec3 initialPosition;
  Vec3 initialVelocity;
  float mass;

  uint8_t *renderData;

  float brightness;
};

struct Physical {
  Physical(const Vec3 &position, const Vec3 &velocity, float mass)
      : position(position), velocity(velocity), mass(mass) {}
  // do not change the signature
  const Vec3 &GetPosition() const { return position; }

  // do not change the signature
  const Vec3 &GetVelocity() const { return velocity; }

  Vec3 position;
  Vec3 velocity;
  float mass;
};

// do not change
struct Renderable {
  explicit Renderable(const uint8_t *renderData);

  std::array<uint8_t, 1024> data;
};

struct Glowing {

  // do not change the signature
  float GetBrightness() const { return brightness; }
  Glowing(float brightness) : brightness(brightness) {}
  float brightness = 1;
};

struct Particle {
  explicit Particle(const ParticleCInfo &pi)
      : physical(pi.initialPosition, pi.initialVelocity, pi.mass),
        renderable(pi.renderData), glowing(pi.brightness) {}

  Physical physical;
  Renderable renderable;
  Glowing glowing;
};

struct ParticleSystem {

  // do not change the signature
  const Physical &GetPhysical(size_t id) const { return physicals[id]; }

  // do not change the signature
  const Renderable &GetRenderable(size_t id) const { return renderables[id]; }

  // do not change the signature
  const Glowing &GetGlowing(size_t id) const { return glowings[id]; }
  // do not change the signature
  size_t CreateParticle(const ParticleCInfo &pi) {
    physicals.emplace_back(pi.initialPosition, pi.initialVelocity, pi.mass);
    renderables.emplace_back(pi.renderData);
    glowings.emplace_back(pi.brightness);
    return physicals.size() - 1;
  }

  // do not change the signature
  void ApplyImpulse(const Vec3 &impulse) {
    const size_t size = physicals.size();
    const float ix = impulse.x;
    const float iy = impulse.y;
    const float iz = impulse.z;

    for (size_t i = 0; i < size; ++i) {
      const float invMass = 1.0f / physicals[i].mass;
      physicals[i].velocity.x += ix * invMass;
      physicals[i].velocity.y += iy * invMass;
      physicals[i].velocity.z += iz * invMass;
    }
  }

  void ClearVelocity() {
    const size_t size = physicals.size();
    for (size_t i = 0; i < size; ++i) {
      physicals[i].velocity = {0, 0, 0};
    }
  }

  // do not change the signature
  void Step(float dt) {
    const size_t size = physicals.size();
    for (size_t i = 0; i < size; ++i) {
      physicals[i].position.x += physicals[i].velocity.x * dt;
      physicals[i].position.y += physicals[i].velocity.y * dt;
      physicals[i].position.z += physicals[i].velocity.z * dt;
    }
  }

  // do not change the signature and the relation with sin
  void StepGlow(float t) {
    const float sin_t = std::sin(t);
    const size_t size = glowings.size();
    for (size_t i = 0; i < size; ++i) {
      glowings[i].brightness = sin_t;
    }
  }

  std::vector<Physical> physicals;
  std::vector<Renderable> renderables;
  std::vector<Glowing> glowings;
  std::vector<Particle> particles;
};


/* ---- Testing part ---- */
Renderable::Renderable(const uint8_t *renderData) {
  if (renderData) {
    std::copy(renderData, renderData + 1024, data.begin());
  } else {
    data.fill(0);
  }
}

int main() {
  const size_t PARTICLE_COUNT = 100'000;
  const size_t ITERATIONS = 1000;

  std::vector<uint8_t> dummyRenderData(1024, 42);
  ParticleSystem system;

  std::cout << "Creating " << PARTICLE_COUNT << " particles..." << std::endl;

  for (size_t i = 0; i < PARTICLE_COUNT; ++i) {
    ParticleCInfo pi;
    pi.initialPosition = Vec3(static_cast<float>(i), 0.0f, 1.0f);
    pi.initialVelocity = Vec3(0.0f, 1.0f, 0.0f);
    pi.mass = 2.0f + static_cast<float>(i % 10);
    pi.renderData = dummyRenderData.data();
    pi.brightness = 1.0f;

    system.CreateParticle(pi);
  }

  std::cout << "Starting benchmark (" << ITERATIONS << " iterations)..."
            << std::endl;

  auto start_time = std::chrono::high_resolution_clock::now();

  float dt = 0.016f; // ~60 FPS
  float time_accumulator = 0.0f;
  Vec3 external_impulse(10.0f, -5.0f, 2.5f);

  for (size_t step = 0; step < ITERATIONS; ++step) {
    time_accumulator += dt;

    system.ApplyImpulse(external_impulse);
    system.Step(dt);
    system.StepGlow(time_accumulator);

    if (step % 100 == 0) {
      system.ClearVelocity();
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count();

  std::cout << "\n=== BENCHMARK RESULTS ===" << std::endl;
  std::cout << "Execution time: " << duration << " ms (" << (duration / 1000.0f)
            << " seconds)" << std::endl;

  size_t test_id = PARTICLE_COUNT / 2;
  const auto &pos = system.GetPhysical(test_id).GetPosition();
  std::cout << "Control Particle [" << test_id << "] Position: X=" << pos.X()
            << ", Y=" << pos.Y() << ", Z=" << pos.Z() << std::endl;
  std::cout << "Control Particle [" << test_id
            << "] Brightness: " << system.GetGlowing(test_id).GetBrightness()
            << std::endl;

  return 0;
}