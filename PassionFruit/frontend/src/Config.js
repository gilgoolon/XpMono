export const API_BASE_URL = 'http://localhost:5000';
export const API_BASE_ENDPOINT = `${API_BASE_URL}/api`;

export const liverUrl = (liverId) => {
    return `${API_BASE_ENDPOINT}/liver/${liverId}`
}
