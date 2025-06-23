import { io } from 'socket.io-client';
import { API_BASE_URL } from './Config.js'; 

// Create and export a single socket instance
export const socket = io(API_BASE_URL, {
    autoConnect: false,
    transports: ['websocket'], // Optional but recommended
});
