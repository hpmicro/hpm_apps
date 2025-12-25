import { h } from 'preact';
import { useState, useEffect } from 'preact/hooks';
import { isConnected, ethercatData } from '../app';
import logoImage from '../assets/HPMicro_Logo.png';

const Welcome = () => {
  const [selectedDigit, setSelectedDigit] = useState(null);

  // 发送数字到数码管
  const sendDigitToDisplay = async (digit) => {
    try {
      const response = await fetch('/api/segment-display', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ digit: digit })
      });

      if (response.ok) {
        setSelectedDigit(digit);
        // 1秒后清除高亮
        setTimeout(() => setSelectedDigit(null), 1000);
      } else {
        console.error('发送数字失败:', response.status);
      }
    } catch (error) {
      console.error('发送数字失败:', error);
    }
  };

  // 获取EtherCAT实际数据
  useEffect(() => {
    if (!isConnected.value) return;

    const fetchEthercatData = async () => {
      try {
        const response = await fetch('/api/ethercat/data');
        if (response.ok) {
          const data = await response.json();
          ethercatData.value = {
            ...ethercatData.value,
            digitalInputs: data.digitalInputs,
            digitalOutputs: data.digitalOutputs,
            syncPeriod: data.syncPeriod,
            syncMode: data.syncMode,
            slaveALStatus: data.slaveALStatus
          };
        } else {
          console.error('EtherCAT数据获取失败:', response.status);
        }
      } catch (error) {
        console.error('EtherCAT数据获取失败:', error);
      }
    };

    // 立即获取一次
    fetchEthercatData();

    // 设置500ms更新周期
    const interval = setInterval(fetchEthercatData, 500);
    return () => clearInterval(interval);
  }, [isConnected.value]);

  return (
    <div class="h-full p-4 relative overflow-hidden flex flex-col">
      {/* 芯片科技背景 - 网格 */}
      <div class="absolute inset-0 pointer-events-none" style={{
        backgroundImage: `
          linear-gradient(rgba(59, 130, 246, 0.03) 1px, transparent 1px),
          linear-gradient(90deg, rgba(59, 130, 246, 0.03) 1px, transparent 1px)
        `,
        backgroundSize: '40px 40px',
        animation: 'grid-fade 4s ease-in-out infinite'
      }}></div>

      {/* 电路点装饰 */}
      <div class="absolute top-20 right-20 w-2 h-2 rounded-full bg-blue-500" style={{
        animation: 'pulse-glow 2s ease-in-out infinite',
        animationDelay: '0s'
      }}></div>
      <div class="absolute top-40 right-40 w-2 h-2 rounded-full bg-blue-500" style={{
        animation: 'pulse-glow 2s ease-in-out infinite',
        animationDelay: '0.5s'
      }}></div>
      <div class="absolute bottom-32 left-32 w-2 h-2 rounded-full bg-blue-500" style={{
        animation: 'pulse-glow 2s ease-in-out infinite',
        animationDelay: '1s'
      }}></div>

      {/* 主标题区和Logo */}
      <div class="mb-4 flex items-center relative z-10">
        <div class="flex-1">
          <div class="flex items-center mb-1">
            <div class="w-1 h-6 mr-3 relative" style={{ background: 'var(--adonis-accent)' }}>
              {/* 脉冲效果 */}
              <div class="absolute inset-0" style={{
                background: 'var(--adonis-accent)',
                animation: 'pulse-glow 2s ease-in-out infinite'
              }}></div>
            </div>
            <h1 class="text-2xl font-bold relative flex items-center" style={{ color: 'var(--adonis-text)' }}>
              HPM5E00EVK 开发板
              {/* 微光效果 */}
              <div class="absolute -bottom-1 left-0 right-0 h-px" style={{
                background: 'linear-gradient(90deg, transparent, var(--adonis-accent), transparent)',
                opacity: '0.3'
              }}></div>
            </h1>
            <div class="flex items-center ml-6 text-sm" style={{ color: 'var(--adonis-text-muted)' }}>
              <div class={`w-2 h-2 rounded-full mr-2`}
                style={{
                  background: isConnected.value ? 'var(--adonis-success)' : 'var(--adonis-danger)',
                  animation: isConnected.value ? 'pulse-glow 2s ease-in-out infinite' : 'none'
                }}></div>
              <span>{isConnected.value ? '设备在线' : '设备离线'}</span>
            </div>
          </div>
        </div>

        {/* Logo区域 */}
        <div class="ml-12 flex-shrink-0">
          <img
            src={logoImage}
            alt="Company Logo"
            class="h-30 w-auto object-contain"
            style={{
              height: '100px'
            }}
          />
        </div>
      </div>

      {/* 主内容区 */}
      <div class="grid grid-cols-1 lg:grid-cols-2 gap-4 max-w-7xl relative z-10 flex-1">

        {/* 左侧：核心亮点 */}
        <div class="space-y-3">
          <h2 class="text-lg font-semibold mb-3 flex items-center relative" style={{ color: 'var(--adonis-text)' }}>
            <svg class="w-6 h-6 mr-2" style={{
              color: 'var(--adonis-accent)',
              animation: 'pulse-glow 2s ease-in-out infinite'
            }} fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M13 10V3L4 14h7v7l9-11h-7z" />
            </svg>
            MCU核心亮点
          </h2>

          {/* 亮点卡片 */}
          {[
            {
              icon: '⚡',
              title: '强劲内核',
              desc: '480MHz RISC-V内核，带DSP扩展',
              metric: '2700+ CoreMark',
              color: '#3B82F6'
            },
            {
              icon: '🌐',
              title: '工业互联核心',
              desc: 'EtherCAT从站 + 千兆以太网',
              metric: '高实时 · 低延迟',
              color: '#10B981'
            },
            {
              icon: '🎯',
              title: '精准运动控制',
              desc: '16路高分辨率PWM输出',
              metric: '100ps 精度',
              color: '#F59E0B'
            },
            {
              icon: '💎',
              title: '高集成度',
              desc: '1MB Flash + 电源管理',
              metric: '单电源供电',
              color: '#8B5CF6'
            },
            {
              icon: '🛡️',
              title: '安全可靠',
              desc: '芯片级安全启动与升级',
              metric: '工业级稳定',
              color: '#EF4444'
            }
          ].map((item, index) => (
            <div key={index} class="adonis-card p-3 hover:translate-x-1 transition-all duration-300 relative overflow-hidden group" style={{
              background: 'var(--adonis-panel-2)',
              border: '1px solid var(--adonis-border)',
              borderLeft: `3px solid ${item.color}`
            }}>
              {/* 悬停时的扫描线效果 */}
              <div class="absolute inset-0 opacity-0 group-hover:opacity-100 transition-opacity duration-500" style={{
                background: `linear-gradient(90deg, transparent, ${item.color}15, transparent)`,
                animation: 'scan 2s linear infinite'
              }}></div>

              <div class="flex items-start relative z-10">
                <div class="text-2xl mr-3 mt-1">{item.icon}</div>
                <div class="flex-1">
                  <div class="flex items-center justify-between mb-1">
                    <h3 class="font-semibold text-sm" style={{ color: 'var(--adonis-text)' }}>
                      {item.title}
                    </h3>
                    <span class="text-xs font-mono px-2 py-0.5 rounded" style={{
                      background: 'rgba(138, 180, 255, 0.1)',
                      color: 'var(--adonis-accent)',
                      border: '1px solid rgba(138, 180, 255, 0.2)'
                    }}>
                      {item.metric}
                    </span>
                  </div>
                  <p class="text-xs" style={{ color: 'var(--adonis-text-muted)' }}>
                    {item.desc}
                  </p>
                </div>
              </div>
            </div>
          ))}
        </div>

        {/* 右侧：控制台内容 */}
        <div class="space-y-3">
          <h2 class="text-lg font-semibold mb-3 flex items-center relative" style={{ color: 'var(--adonis-text)' }}>
            <svg class="w-6 h-6 mr-2" style={{
              color: 'var(--adonis-accent)',
              animation: 'pulse-glow 2s ease-in-out infinite'
            }} fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M9.75 17L9 20l-1 1h8l-1-1-.75-3M3 13h18M5 17h14a2 2 0 002-2V5a2 2 0 00-2-2H5a2 2 0 00-2 2v10a2 2 0 002 2z" />
            </svg>
            实时控制
          </h2>

          {/* 数字量输入监控 */}
          {typeof ethercatData.value?.digitalInputs === 'number' && (
            <div class="adonis-card p-4">
              <h2 class="text-base font-semibold adonis-section-title mb-3">
                数字量输入监控
              </h2>
              <p class="text-xs mb-3" style={{ color: 'var(--adonis-text-muted)' }}>
                按下开发板上的物理按键，对应的按钮指示器将实时点亮（显示为1），松开按键后熄灭（显示为0）
              </p>
              <div class="grid grid-cols-6 gap-3">
                {Array.from({ length: 6 }, (_, index) => {
                  const state = (ethercatData.value.digitalInputs >> index) & 1;
                  return (
                    <div key={index} class="flex flex-col items-center">
                      <div class={`relative w-10 h-10 rounded-lg flex items-center justify-center mb-1 overflow-hidden`}
                        style={{
                          background: state === 1
                            ? 'linear-gradient(135deg, #3b82f6 0%, #1d4ed8 100%)'
                            : 'linear-gradient(135deg, var(--adonis-panel-2) 0%, var(--adonis-panel) 100%)',
                          border: '1px solid',
                          borderColor: state === 1 ? '#3b82f6' : 'var(--adonis-border)',
                          boxShadow: state === 1
                            ? '0 0 8px rgba(59, 130, 246, 0.3), inset 0 1px 0 rgba(255, 255, 255, 0.1)'
                            : '0 2px 4px rgba(0, 0, 0, 0.1), inset 0 1px 0 rgba(255, 255, 255, 0.05)'
                        }}>
                        <span class="text-base font-mono font-bold tracking-wider" style={{
                          color: state === 1 ? '#ffffff' : 'var(--adonis-accent)',
                          textShadow: 'none'
                        }}>
                          {state}
                        </span>
                        <div class="absolute inset-0 rounded-lg opacity-20" style={{
                          background: state === 1
                            ? 'linear-gradient(45deg, transparent 30%, rgba(59, 130, 246, 0.2) 50%, transparent 70%)'
                            : 'linear-gradient(45deg, transparent 30%, rgba(59, 130, 246, 0.05) 50%, transparent 70%)',
                          animation: state === 1 ? 'scan 3s linear infinite' : 'none'
                        }}></div>
                      </div>
                      <span class="text-xs font-mono" style={{ color: 'var(--adonis-text-muted)' }}>IN{index + 1}</span>
                    </div>
                  );
                })}
              </div>
            </div>
          )}

          {/* 数码管控制区 */}
          <div class="adonis-card p-4">
            <h2 class="text-base font-semibold adonis-section-title mb-3">
              数码管显示控制
            </h2>
            <p class="text-xs mb-3" style={{ color: 'var(--adonis-text-muted)' }}>
              点击数字按钮，数码管将显示对应数字
            </p>
            <div class="grid grid-cols-10 gap-2">
              {Array.from({ length: 10 }, (_, index) => (
                <button
                  key={index}
                  onClick={() => sendDigitToDisplay(index)}
                  class={`relative h-12 rounded-lg font-mono text-lg font-bold transition-all duration-200 ${selectedDigit === index ? 'scale-95' : 'hover:scale-105'
                    }`}
                  style={{
                    background: selectedDigit === index
                      ? 'linear-gradient(135deg, #3b82f6 0%, #1d4ed8 100%)'
                      : 'linear-gradient(135deg, var(--adonis-panel-2) 0%, var(--adonis-panel) 100%)',
                    border: '1px solid',
                    borderColor: selectedDigit === index ? '#3b82f6' : 'var(--adonis-border)',
                    color: selectedDigit === index ? '#ffffff' : 'var(--adonis-text)',
                    boxShadow: selectedDigit === index
                      ? '0 0 12px rgba(59, 130, 246, 0.5), inset 0 1px 0 rgba(255, 255, 255, 0.1)'
                      : '0 2px 4px rgba(0, 0, 0, 0.1)',
                    cursor: 'pointer'
                  }}
                >
                  {index}
                  {selectedDigit === index && (
                    <div class="absolute inset-0 rounded-lg opacity-30" style={{
                      background: 'linear-gradient(45deg, transparent 30%, rgba(255, 255, 255, 0.3) 50%, transparent 70%)',
                      animation: 'scan 1s linear infinite'
                    }}></div>
                  )}
                </button>
              ))}
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Welcome;
