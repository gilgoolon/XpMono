import React, { useState, useEffect } from 'react';
import {
  Box,
  List,
  ListItem,
  ListItemButton,
  ListItemText,
  Typography,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Button,
  Paper
} from '@mui/material';

import { API_BASE_URL } from "../Config.js";

export default function CommandTemplates({ onSelectTemplate }) {
  const [templates, setTemplates] = useState([]);
  const [selectedTemplate, setSelectedTemplate] = useState(null);
  const [previewOpen, setPreviewOpen] = useState(false);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchTemplates = async () => {
      try {
        const response = await fetch(`${API_BASE_URL}/api/command-templates`);
        if (!response.ok) {
          throw new Error('Network response was not ok');
        }
        const data = await response.json();
        setTemplates(data);
      } catch (err) {
        setError(err.message);
      } finally {
        setLoading(false);
      }
    };

    fetchTemplates();
  }, []);

  if (loading) return <div>Loading...</div>;
  if (error) return <div>Error: {error}</div>;

  const handlePreview = (template) => {
    setSelectedTemplate(template);
    setPreviewOpen(true);
  };

  const handleUseTemplate = () => {
    if (selectedTemplate) {
      onSelectTemplate(JSON.stringify(selectedTemplate.content, null, 2));
      setPreviewOpen(false);
    }
  };

  return (
    <Box sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
      <Typography variant="subtitle2" sx={{ p: 1 }}>
        Command Templates
      </Typography>
      <Paper variant="outlined" sx={{ flex: 1, overflow: 'auto' }}>
        <List dense>
          {templates.map((template) => (
            <ListItem key={template.name} disablePadding>
              <ListItemButton onClick={() => handlePreview(template)}>
                <ListItemText primary={template.name} />
              </ListItemButton>
            </ListItem>
          ))}
        </List>
      </Paper>

      <Dialog
        open={previewOpen}
        onClose={() => setPreviewOpen(false)}
        maxWidth="md"
        fullWidth
      >
        <DialogTitle>
          {selectedTemplate?.name}
        </DialogTitle>
        <DialogContent>
          <pre style={{ whiteSpace: 'pre-wrap', wordBreak: 'break-word' }}>
            {selectedTemplate && JSON.stringify(selectedTemplate.content, null, 2)}
          </pre>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setPreviewOpen(false)}>Cancel</Button>
          <Button onClick={handleUseTemplate} variant="contained">
            Use Template
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}
