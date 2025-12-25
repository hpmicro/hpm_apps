// @ts-nocheck
import { Router } from 'preact-router';
import { signal } from '@preact/signals';
import { useEffect, useRef } from 'preact/hooks';
import Welcome from './pages/Welcome';

// 全局状态
export const isConnected = signal(false);
export const deviceInfo = signal({});

// 主题状态管理
export const isDarkTheme = signal(() => {
  const saved = localStorage.getItem('adonis-theme');
  if (saved) {
    return saved === 'dark';
  }
  return window.matchMedia('(prefers-color-scheme: dark)').matches;
});

// 主题切换函数
export const toggleTheme = () => {
  // 添加全局过渡动画
  document.body.classList.add('theme-transitioning');

  isDarkTheme.value = !isDarkTheme.value;
  localStorage.setItem('adonis-theme', isDarkTheme.value ? 'dark' : 'light');
  applyTheme();

  // 移除动画类
  setTimeout(() => {
    document.body.classList.remove('theme-transitioning');
  }, 300);
};

// 应用主题到DOM
export const applyTheme = () => {
  const root = document.documentElement;
  if (isDarkTheme.value) {
    root.classList.add('dark');
    root.classList.remove('light');
  } else {
    root.classList.add('light');
    root.classList.remove('dark');
  }
};

// EtherCAT相关状态
export const ethercatData = signal({
  digitalInputs: 0,        // 16位数字输入 (位掩码)
  digitalOutputs: 0,       // 16位数字输出 (位掩码)
  syncPeriod: 0,           // 同步周期 (μs)
  dcSyncStatus: 'INIT',    // 同步状态: INIT, PREOP, SAFEOP, OP
  syncMode: 'FreeRun',     // 同步模式: Freerun, SM, DC
  slaveALStatus: 'INIT',   // 从站状态
});

export function App() {
  const heartbeatTimer = useRef(null);
  const heartbeatTimeout = useRef(null);
  const isHeartbeatPending = useRef(false);

  // 初始化主题
  useEffect(() => {
    applyTheme();
  }, []);

  // 心跳检测函数
  const checkHeartbeat = async () => {
    // 如果上一次心跳还未完成，则不发送新的心跳
    if (isHeartbeatPending.current) {
      return;
    }

    isHeartbeatPending.current = true;

    try {
      const controller = new AbortController();
      heartbeatTimeout.current = setTimeout(() => {
        controller.abort();
      }, 3000); // 3秒超时

      const response = await fetch("/api/heartbeat", {
        signal: controller.signal
      });

      if (response.status === 403) {
        // isConnected.value = false;
      } else {
        const data = await response.json();
        isConnected.value = true;
        deviceInfo.value = data;
      }
    } catch (error) {
      if (error.name === 'AbortError') {
        console.log('心跳检测超时');
      } else {
        console.error('心跳检测错误:', error);
      }
    } finally {
      clearTimeout(heartbeatTimeout.current);
      isHeartbeatPending.current = false;
    }
  };

  // 心跳检测
  useEffect(() => {
    checkHeartbeat(); // 立即执行一次
    heartbeatTimer.current = setInterval(checkHeartbeat, 1000); // 每秒执行一次

    return () => {
      clearInterval(heartbeatTimer.current);
      clearTimeout(heartbeatTimeout.current);
    };
  }, []);

  // 轮询按键状态，更新 EtherCAT 数字输入显示
  useEffect(() => {
    let stopped = false;
    const pollDI = async () => {
      try {
        const res = await fetch('/api/di');
        if (!res.ok) return;
        const data = await res.json();
        // 更新 EtherCAT 数字输入状态（按键映射到 IN1-IN6）
        if (!stopped && data && typeof data.digitalInputs === 'number') {
          ethercatData.value = {
            ...ethercatData.value,
            digitalInputs: data.digitalInputs
          };
        }
      } catch (e) {
        // ignore errors
      }
    };
    const id = setInterval(pollDI, 200); // 200ms 轮询
    pollDI();
    return () => {
      stopped = true;
      clearInterval(id);
    };
  }, []);

  return (
    <div class="h-screen overflow-x-hidden overflow-y-auto">
      <main class="w-full">
        <Router>
          <Welcome path="/" />
        </Router>
      </main>
    </div>
  );
} 